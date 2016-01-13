#include <stdio.h>
#include <string.h>
#include "expression.h"
#include "coroutine/coroutine.h"
#include "interface/engine.h"

/* interrupt signal */
InterruptSignal CGC_interrupt_signal = INTER_NONE;

Ink_NumericValue Ink_NumericConstant::parseNumeric(string code, bool *is_success)
{
	unsigned long val = 0;
	double fval = 0.0;
	int flag = 1;

	/* is minus */
	if (code[0] == '-') {
		flag = -1;
		code = code.substr(1);
	}

	/* is float */
	if (sscanf(code.c_str(), "%lf", &fval) > 0) {
		if (is_success)
			*is_success = true;
		return fval * flag;
	}

	/* is integer */
	if (sscanf(code.c_str(), "0x%lx", &val) > 0
		|| sscanf(code.c_str(), "0X%lX", &val) > 0
		|| sscanf(code.c_str(), "0%lo", &val) > 0
		|| sscanf(code.c_str(), "%lu", &val) > 0) {
		if (is_success)
			*is_success = true;
		return val * flag;
	}

	if (is_success)
		*is_success = false;

	return 0.0;
}

Ink_Expression *Ink_NumericConstant::parse(string code)
{
	bool is_success = false;
	Ink_NumericValue val = Ink_NumericConstant::parseNumeric(code, &is_success);

	if (is_success)
		return new Ink_NumericConstant(val);

	return NULL;
}

extern IGC_CollectEngine *ink_sync_call_tmp_engine;

Ink_Object *Ink_YieldExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;

	// no temp engine means no coroutine
	if (!ink_sync_call_tmp_engine) {
		InkErr_Yield_Without_Coroutine();
		// unreachable
	}

	Ink_Object *ret = ret_val ? ret_val->eval(context_chain) : new Ink_NullObject();
	if (INTER_SIGNAL_RECEIVED) {
		RESTORE_LINE_NUM;
		return CGC_interrupt_value;
	}
	CGC_interrupt_value = ret;

	unsigned int self_layer = getCurrentLayer();
	int self_id = getThreadID();
	IGC_CollectEngine *engine_backup = Ink_getCurrentEngine()->getCurrentGC();

	printf("***Coroutine yield: id %d at layer %u\n", self_id, self_layer);

	pthread_mutex_lock(&ink_sync_call_mutex);
	InkCoCall_switchCoroutine();
	pthread_mutex_unlock(&ink_sync_call_mutex);

REWAIT:
	do {
		while (ink_sync_call_current_thread != self_id) ;
	} while (getCurrentLayer() != self_layer);
	if (ink_sync_call_current_thread != self_id) goto REWAIT;

	IGC_initGC(engine_backup);

	RESTORE_LINE_NUM;
	return CGC_interrupt_value;
}

Ink_Object *Ink_InterruptExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;

	Ink_Object *ret = ret_val ? ret_val->eval(context_chain) : new Ink_NullObject();
	if (INTER_SIGNAL_RECEIVED) {
		RESTORE_LINE_NUM;
		return CGC_interrupt_value;
	}

	RESTORE_LINE_NUM;

	CGC_interrupt_value = ret;
	CGC_interrupt_signal = signal;

	return ret;
}

Ink_Object *Ink_LogicExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	bool ret_val = false;
	SET_LINE_NUM;

	/* first eval left hand side */
	Ink_Object *lhs = lval->eval(context_chain);
	Ink_Object *rhs;

	/* interrupt signal received */
	if (INTER_SIGNAL_RECEIVED) {
		RESTORE_LINE_NUM;
		return CGC_interrupt_value;
	}

	if (isTrue(lhs)) {
		/* left hand side true */
		if (type == LOGIC_OR)
			/* if operator is or, return true directly */
			ret_val = true;
		else {
			/* if operator is and, make sure the right hand side is true, too */
			rhs = rval->eval(context_chain);
			if (INTER_SIGNAL_RECEIVED) {
				RESTORE_LINE_NUM;
				return CGC_interrupt_value;
			}
			if (isTrue(rhs)) {
				ret_val = true;
			}
		}
	} else {
		/* if operator is or, judge right hand side */
		if (type == LOGIC_OR && isTrue(rval->eval(context_chain))) {
			if (INTER_SIGNAL_RECEIVED) {
				RESTORE_LINE_NUM;
				return CGC_interrupt_value;
			}
			ret_val = true;
		}
	}

	RESTORE_LINE_NUM;

	return new Ink_Numeric(ret_val);
}

Ink_Object *Ink_AssignmentExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;

	Ink_Object *rval_ret;
	Ink_Object *lval_ret;
	Ink_Object **tmp;

	rval_ret = rval->eval(context_chain);
	/* eval right hand side first */
	if (INTER_SIGNAL_RECEIVED) {
		RESTORE_LINE_NUM;
		return CGC_interrupt_value;
	}

	lval_ret = lval->eval(context_chain, Ink_EvalFlag(true));
	/* left hand side next */
	if (INTER_SIGNAL_RECEIVED) {
		RESTORE_LINE_NUM;
		return CGC_interrupt_value;
	}

	if (lval_ret->address) {
		if (lval_ret->address->setter) { /* if has setter, call it */
			tmp = (Ink_Object **)malloc(sizeof(Ink_Object *));
			tmp[0] = rval_ret;
			lval_ret->address->setValue(lval_ret->address->setter->call(context_chain, 1, tmp, lval_ret));
			CGC_interrupt_signal = INTER_NONE;
			free(tmp);
		} else {
			/* no setter, directly assign */
			lval_ret->address->setValue(rval_ret);
		}
		return is_return_lval ? lval_ret : rval_ret;
	}

	InkErr_Assigning_Unassignable_Expression();
	abort();

	RESTORE_LINE_NUM;
}

Ink_Object *Ink_HashTableExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;

	Ink_Object *ret = new Ink_Object(), *key;
	unsigned int i;

	for (i = 0; i < mapping.size(); i++) {
		/* two possibility: 1. identifier key; 2. expression key with brackets */
		if (mapping[i]->name) {
			ret->setSlot(mapping[i]->name->c_str(), mapping[i]->value->eval(context_chain));
			if (INTER_SIGNAL_RECEIVED) {
				RESTORE_LINE_NUM;
				return CGC_interrupt_value;
			}
		} else {
			key = mapping[i]->key->eval(context_chain);
			if (INTER_SIGNAL_RECEIVED) {
				RESTORE_LINE_NUM;
				return CGC_interrupt_value;
			}
			if (key->type != INK_STRING) {
				InkWarn_Hash_Table_Mapping_Expect_String();
				return new Ink_NullObject();
			}
			ret->setSlot(as<Ink_String>(key)->value.c_str(), mapping[i]->value->eval(context_chain));
			if (INTER_SIGNAL_RECEIVED) {
				RESTORE_LINE_NUM;
				return CGC_interrupt_value;
			}
		}
	}

	RESTORE_LINE_NUM;
	return ret;
}

Ink_Object *Ink_TableExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;

	Ink_ArrayValue val = Ink_ArrayValue();
	unsigned int i;

	for (i = 0; i < elem_list.size(); i++) {
		val.push_back(new Ink_HashTable("", elem_list[i]->eval(context_chain)));
		if (INTER_SIGNAL_RECEIVED) {
			RESTORE_LINE_NUM;
			Ink_Array::disposeArrayValue(val);
			return CGC_interrupt_value;
		}
	}

	RESTORE_LINE_NUM;
	return new Ink_Array(val);
}

Ink_Object *Ink_HashExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;

	Ink_Object *base_obj = base->eval(context_chain);
	if (INTER_SIGNAL_RECEIVED) {
		RESTORE_LINE_NUM;
		return CGC_interrupt_value;
	}

	RESTORE_LINE_NUM;
	return getSlot(context_chain, base_obj, slot_id->c_str(), flags);
}

Ink_HashExpression::ProtoSearchRet Ink_HashExpression::searchPrototype(Ink_Object *obj, const char *id)
{
	Ink_HashTable *hash = obj->getSlotMapping(id);
	Ink_HashTable *proto;
	Ink_Object *proto_obj = NULL;
	Ink_HashExpression::ProtoSearchRet search_res;

	if (!hash) { /* cannot find slot in object itself */
		/* get prototype */
		proto = obj->getSlotMapping("prototype");

		/* prototype exists and it's not undefined */
		if (proto && proto->getValue()->type != INK_UNDEFINED) {
			/* search the slot in prototype, and get the result */
			hash = (search_res = searchPrototype(proto->getValue(), id)).hash;
			proto_obj = search_res.base;
		}
	}

	/* return result with base pointed to the prototype(if has)
	 * in which found the slot
	 */
	return Ink_HashExpression::ProtoSearchRet(hash, proto_obj ? proto_obj : obj);
}

Ink_Object *Ink_HashExpression::getSlot(Ink_ContextChain *context_chain, Ink_Object *obj, const char *id, Ink_EvalFlag flags)
{
	Ink_HashTable *hash, *address;
	Ink_Object *base = obj, *ret, *tmp;
	Ink_Object **argv;
	ProtoSearchRet search_res;

	if (obj->type == INK_UNDEFINED) {
		InkWarn_Get_Undefined_Hash();
	}

	if (!(hash = obj->getSlotMapping(id)) /* cannot find slot in the origin object */) {
		/* search prototype */
		hash = (search_res = searchPrototype(obj, id)).hash;

		/* create barrier to prevent changes on prototype */
		address = obj->setSlot(id, NULL);
		if (hash) { /* if found the slot in prototype */
			base = search_res.base; /* set base as the prototype(to make some native method run correctly) */
			ret = hash->getValue();
		} else {
			if ((tmp = obj->getSlot("missing"))->type == INK_FUNCTION) {
				/* has missing method, call it */
				argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
				argv[0] = new Ink_String(string(id));
				ret = tmp->call(context_chain, 1, argv);
				free(argv);
			} else {
				/* return undefined */
				ret = new Ink_Undefined();
			}
		}
	} else {
		/* found slot correctly */
		ret = hash->getValue();
		address = hash;
	}

	/* set address for possible assignment */
	ret->address = address;
	/* set base */
	const char *debug_name_back = base->getDebugName();
	ret->setSlot("base", base);
	base->setDebugName(debug_name_back);

	/* call getter if has one */
	if (!flags.is_left_value && address->getter) {
		ret = address->getter->call(context_chain, 0, NULL, ret);
		/* trap all interrupt signal */
		CGC_interrupt_signal = INTER_NONE;
	}

	return ret;
}

Ink_Object *Ink_FunctionExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;
	RESTORE_LINE_NUM;
	return new Ink_FunctionObject(param, exp_list, context_chain->copyContextChain(), is_inline, is_generator);
}

/* expand argument -- expand array object to parameter */
inline Ink_ArgumentList expandArgument(Ink_Object *obj)
{
	Ink_ArgumentList ret = Ink_ArgumentList();
	Ink_ArrayValue arr_val;
	unsigned int i;

	if (!obj || obj->type != INK_ARRAY) {
		InkWarn_With_Attachment_Require();
		return ret;
	}

	arr_val = as<Ink_Array>(obj)->value;

	for (i = 0; i < arr_val.size(); i++) {
		ret.push_back(new Ink_Argument(new Ink_ShellExpression(arr_val[i]->getValue())));
	}
	return ret;
}

Ink_Object *Ink_CallExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;

	unsigned int i;
	Ink_Object **argv = NULL;
	Ink_Object *ret_val, *expandee;
	/* eval callee to get parameter declaration */
	Ink_Object *func = callee->eval(context_chain);
	if (INTER_SIGNAL_RECEIVED)
		return CGC_interrupt_value;
	Ink_ParamList param_list = Ink_ParamList();
	Ink_ArgumentList dispose_list, tmp_arg_list, another_tmp_arg_list;

	if (func->type == INK_FUNCTION) {
		param_list = as<Ink_FunctionObject>(func)->param;
	}
	if (is_new) {
		func = Ink_HashExpression::getSlot(context_chain, func, "new");
	}

	for (i = 0, tmp_arg_list = Ink_ArgumentList();
		 i < arg_list.size(); i++) {
		if (arg_list[i]->is_expand) {
			/* if the argument is 'with' argument attachment */

			/* eval expandee */
			expandee = arg_list[i]->expandee->eval(context_chain);
			if (INTER_SIGNAL_RECEIVED) {
				ret_val = CGC_interrupt_value;
				goto DISPOSE_LIST;
			}

			/* expand argument */
			another_tmp_arg_list = expandArgument(expandee);

			/* insert expanded argument to dispose list and temporary argument list */
			dispose_list.insert(dispose_list.end(), another_tmp_arg_list.begin(), another_tmp_arg_list.end());
			tmp_arg_list.insert(tmp_arg_list.end(), another_tmp_arg_list.begin(), another_tmp_arg_list.end());
		} else {
			/* normal argument, directly push to argument list */
			tmp_arg_list.push_back(arg_list[i]);
		}
	}

	if (tmp_arg_list.size()) {
		/* allocate argument list */
		argv = (Ink_Object **)malloc(tmp_arg_list.size() * sizeof(Ink_Object *));

		/* set argument list, according to the parameter declaration */
		for (i = 0; i < tmp_arg_list.size(); i++) {
			if (i < param_list.size() && param_list[i].is_ref) {
				/* if the paramete is declared as reference, seal the expression to a anonymous function */
				if (param_list[i].is_variant) {
					for (; i < tmp_arg_list.size(); i++) {
						Ink_ExpressionList exp_list = Ink_ExpressionList();
						exp_list.push_back(tmp_arg_list[i]->arg);
						argv[i] = new Ink_FunctionObject(Ink_ParamList(), exp_list,
														 context_chain->copyContextChain(),
														 true);
					}
				} else {
					Ink_ExpressionList exp_list = Ink_ExpressionList();
					exp_list.push_back(tmp_arg_list[i]->arg);
					argv[i] = new Ink_FunctionObject(Ink_ParamList(), exp_list,
													 context_chain->copyContextChain(),
													 true);
				}
			} else {
				/* normal argument */
				argv[i] = tmp_arg_list[i]->arg->eval(context_chain);
				if (INTER_SIGNAL_RECEIVED) {
					ret_val = CGC_interrupt_value;
					/* goto dispose and interrupt */
					goto DISPOSE_ARGV;
				}
			}
		}
	}

	ret_val = func->call(context_chain, tmp_arg_list.size(), argv);

DISPOSE_ARGV:
	free(argv);

DISPOSE_LIST:
	for (i = 0; i < dispose_list.size(); i++) {
		delete dispose_list[i];
	}

	RESTORE_LINE_NUM;

	return ret_val;
}

Ink_Object *Ink_IdentifierExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;

	Ink_Object *ret;
	ret = getContextSlot(context_chain, id->c_str(), context_type, flags, if_create_slot);

	RESTORE_LINE_NUM;
	return ret;
}

Ink_Object *Ink_IdentifierExpression::getContextSlot(Ink_ContextChain *context_chain, const char *name,
													 IDContextType context_type, Ink_EvalFlag flags, bool if_create_slot)
{
	/* Variables */
	Ink_HashTable *hash, *missing;
	Ink_ContextChain *local = context_chain->getLocal();
	Ink_ContextChain *global = context_chain->getGlobal();
	Ink_ContextChain *dest_context = local;
	Ink_Object *tmp, *ret;
	Ink_Object **argv;

	/* Determine the type of reference:
	 * 1. local
	 *		find slot in the local context
	 * 2. global
	 *		find slot in the global context
	 * 3. default
	 *		search all contexts to find slot
	 */
	switch (context_type) {
		case ID_LOCAL:
			hash = local->context->getSlotMapping(name);
			missing = local->context->getSlotMapping("missing");
			break;
		case ID_GLOBAL:
			hash = global->context->getSlotMapping(name);
			missing = global->context->getSlotMapping("missing");
			dest_context = global;
			break;
		default:
			hash = context_chain->searchSlotMapping(name);
			missing = context_chain->searchSlotMapping("missing");
			break;
	}

	/* if the slot cannot be found */
	if (!hash) {
		if (if_create_slot) { /* if has the "var" keyword */
			ret = new Ink_Object();
			hash = dest_context->context->setSlot(name, ret);
		} else { /* generate a undefined value */
			if (missing && missing->getValue()->type == INK_FUNCTION) {
				argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
				argv[0] = new Ink_String(name);
				ret = missing->getValue()->call(context_chain, 1, argv);
				free(argv);
			} else {
				ret = new Ink_Undefined();
			}
			hash = dest_context->context->setSlot(name, NULL);
		}
	} else {
		ret = hash->getValue(); /* get value */
	}
	ret->address = hash; /* set its address for assigning */

	/* if it's not a left value reference(which will call setter in assign exp) and has getter, call it */
	if (!flags.is_left_value && hash->getter) {
		tmp = hash->getter->call(context_chain, 0, NULL,
								 hash->getValue(),
								 /* don't return "this" pointer anyway */
								 false);

		/* trap all interrupt signal */
		CGC_interrupt_signal = INTER_NONE;

		return tmp;
	}
	// hash->value->setSlot("this", hash->value);

	return ret;
}

Ink_Object *Ink_ArrayLiteral::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;
	Ink_ArrayValue val = Ink_ArrayValue();
	unsigned int i;

	for (i = 0; i < elem_list.size(); i++) {
		val.push_back(new Ink_HashTable("", elem_list[i]->eval(context_chain)));
		if (INTER_SIGNAL_RECEIVED) {
			RESTORE_LINE_NUM;
			Ink_Array::disposeArrayValue(val);
			return CGC_interrupt_value;
		}
	}

	RESTORE_LINE_NUM;
	return new Ink_Array(val);
}
