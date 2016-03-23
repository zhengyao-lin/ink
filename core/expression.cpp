#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "error.h"
#include "object.h"
#include "context.h"
#include "expression.h"
#include "protocol.h"
#include "general.h"
#include "gc/collect.h"
#include "coroutine/coroutine.h"
#include "interface/engine.h"

#define SET_LINE_NUM (file_name_back = engine->current_file_name, \
					  line_num_back = engine->current_line_number, \
					  engine->current_file_name = (file_name ? file_name : engine->current_file_name), \
					  engine->current_line_number = (line_number >= 0 ? line_number : engine->current_line_number))
#define RESTORE_LINE_NUM (engine->current_file_name = file_name_back, engine->current_line_number = line_num_back)
#define INTER_SIGNAL_RECEIVED (engine->getSignal() != INTER_NONE)
#define CATCH_SIGNAL_RET do { \
	if (INTER_SIGNAL_RECEIVED) { \
		RESTORE_LINE_NUM; \
		return engine->getInterruptValue(); \
	} \
} while (0)

#define IS_BIN(c) ((c) == '0' || (c) == '1')
#define IS_DEC(c) ((c) <= '9' && (c) >= '0')
#define IS_HEX(c) (IS_DEC(c) || ((c) <= 'f' && (c) >= 'a') || ((c) <= 'F' && (c) >= 'A'))
#define IS_OCT(c) ((c) <= '7' && (c) >= '0')

#define IS_CAPITAL(c) ((c) <= 'Z' && (c) >= 'A')
#define TO_LOWER(c) (IS_CAPITAL(c) ? 'a' + (c) - 'A' : (c))

#define DEC_TO_NUM(c) ((c) - '0')
#define BIN_TO_NUM(c) (DEC_TO_NUM(c))
#define HEX_TO_NUM(c) (IS_DEC(c) ? DEC_TO_NUM(c) : TO_LOWER(c) - 'a' + 10)
#define OCT_TO_NUM(c) (DEC_TO_NUM(c))

#define IS_LEGAL(mode, c) (IS_DEC(c) ? (c) < ('0' + (mode)) \
									 : (mode == HEX && IS_HEX(c) ? (TO_LOWER(c) - 'a') < 6 : false))

#define TO_NUM(mode, c) ((mode) <= 10 ? DEC_TO_NUM(c) : HEX_TO_NUM(c))

namespace ink {

using namespace std;

Ink_NumericValue Ink_NumericExpression::parseNumeric(string code, bool *is_success)
{
	double ret = 0.0;
	double ret_back = 0.0;
	bool use_e = false;
	string::size_type i;
	int flag = 1, flag_back = 1, decimal = 0;
	enum {
		BIN = 2,
		OCT = 8,
		DEC = 10,
		HEX = 16
	} mode = DEC;

	if (is_success)
		*is_success = true;

	if (code.length()) {
		if (code[0] == '-') {
			flag = -1;
			code = code.substr(1);
		} else if (code[0] == '+') {
			flag = 1;
			code = code.substr(1);
		}
	}

	if (code.length() && code[0] == '0') {
		code = code.substr(1);
		if (code.length()) {
			if (code[0] == 'x' || code[0] == 'X') {
				mode = HEX;
				code = code.substr(1);
			} else if (code[0] == 'o' || code[0] == 'O') {
				mode = OCT;
				code = code.substr(1);
			} else if (code[0] == 'b' || code[0] == 'B') {
				mode = BIN;
				code = code.substr(1);
			}
		}
	}

	for (i = 0; i < code.length(); i++) {
		if (code[i] == '.') {
			decimal++;
			continue;
		} else if (code[i] == 'e'
				   || code[i] == 'E') {
			ret_back = ret;
			flag_back = flag;
			ret = 0.0;
			flag = 1;
			decimal = 0;
			use_e = true;
			i++;
			if (i < code.length()) {
				if (code[i] == '-') {
					flag = -1;
				} else if (code[i] == '+') {
					flag = 1;
				} else {
					i--;
				}
				continue;
			}

			fprintf(stderr, "No exponent given\n");
			break;
		}

		if (IS_LEGAL(mode, code[i])) {
			if (decimal > 0) {
				ret += TO_NUM(mode, code[i]) / pow((int)mode, decimal);
				decimal++;
			} else {
				ret = ret * mode + TO_NUM(mode, code[i]);
			}
		} else {
			fprintf(stderr, "Illegal character \'%c\'\n", code[i]);
			if (is_success)
				*is_success = false;
			break;
		}
	}

	ret = use_e ? ret_back * flag_back * pow(10, ret * flag) : ret * flag;

	return ret == (Ink_SInt64)ret ? Ink_NumericValue((Ink_SInt64)ret) : Ink_NumericValue(ret);
}

Ink_Expression *Ink_NumericExpression::parse(string code)
{
	bool is_success = false;
	Ink_NumericValue val = Ink_NumericExpression::parseNumeric(code, &is_success);

	if (is_success)
		return new Ink_NumericExpression(val);
	else
	
	return NULL;
}

Ink_Object *Ink_CommaExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	SET_LINE_NUM;

	Ink_Object *ret = NULL_OBJ;
	Ink_ExpressionList::size_type i;

	for (i = 0; i < exp_list.size(); i++) {
		ret = exp_list[i]->eval(engine, context_chain);
		CATCH_SIGNAL_RET;
	}

	RESTORE_LINE_NUM;
	return ret;
}

Ink_Object *Ink_YieldExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	SET_LINE_NUM;

	InkCoro_Scheduler *sched;

	if (!(sched = engine->currentScheduler())) {
		InkError_Yield_Without_Coroutine(engine);
		return NULL_OBJ;
	}

	Ink_Object *ret = ret_val ? ret_val->eval(engine, context_chain) : NULL_OBJ;
	CATCH_SIGNAL_RET;

	engine->setInterruptValue(ret);

	IGC_CollectEngine *gc_engine_backup = engine->getCurrentGC();

	sched->yield();

	engine->setCurrentGC(gc_engine_backup);

	RESTORE_LINE_NUM;
	return engine->getInterruptValue();
}

Ink_Object *Ink_InterruptExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	Ink_InterruptSignal tmp_sig;
	SET_LINE_NUM;

	Ink_Object *ret = ret_val ? ret_val->eval(engine, context_chain) : NULL_OBJ;
	CATCH_SIGNAL_RET;

	RESTORE_LINE_NUM;

	if (custom_sig) {
		tmp_sig = engine->getCustomInterruptSignal(*custom_sig);
		if (!tmp_sig) {
			InkWarn_Undefined_Custom_Interrupt_Name(engine, custom_sig->c_str());
			return ret;
		}
		engine->setInterrupt(tmp_sig, ret);
		return ret;
	}

	engine->setInterrupt(sig, ret);

	return ret;
}

Ink_Object *Ink_LogicExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	Ink_Object *ret = NULL;

	SET_LINE_NUM;

	/* first eval left hand side */
	Ink_Object *lhs = lval->eval(engine, context_chain);
	Ink_Object *rhs;

	/* interrupt signal received */
	CATCH_SIGNAL_RET;

	// logic and/or: return the value that determines the true/false
	if (type == INK_LOGIC_AND) {
		if (isTrue(lhs)) {
			ret = rhs = rval->eval(engine, context_chain);
			CATCH_SIGNAL_RET;
		} else {
			ret = lhs;
		}
	} else if (type == INK_LOGIC_OR) {
		if (isTrue(lhs)) {
			ret = lhs;
		} else {
			ret = rhs = rval->eval(engine, context_chain);
			CATCH_SIGNAL_RET;
		}
	} else {
		// usually unreachable
		assert(type == INK_LOGIC_AND || type == INK_LOGIC_OR);
	}

	RESTORE_LINE_NUM;

	return ret;
}

Ink_Object *Ink_AssignmentExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	SET_LINE_NUM;

	Ink_Object *rval_ret;
	Ink_Object *lval_ret;
	Ink_Object **tmp;
	Ink_Object *ret;
	Ink_Object *assign_method, *assign_event;

	rval_ret = rval->eval(engine, context_chain);
	/* eval right hand side first */
	CATCH_SIGNAL_RET;

	lval_ret = lval->eval(engine, context_chain, Ink_EvalFlag(true));
	/* left hand side next */
	CATCH_SIGNAL_RET;

	if ((assign_event = lval_ret->getSlot(engine, "@assign"))->type == INK_FUNCTION) {
		tmp = (Ink_Object **)malloc(sizeof(Ink_Object *));
		tmp[0] = rval_ret;
		// assign_method->setSlot_c("base", lval_ret);
		assign_event->setBase(lval_ret);
		ret = assign_event->call(engine, context_chain, 1, tmp);
		free(tmp);
		CATCH_SIGNAL_RET;
	}

	if ((assign_method = lval_ret->getSlot(engine, "="))->type == INK_FUNCTION) {
		tmp = (Ink_Object **)malloc(sizeof(Ink_Object *));
		tmp[0] = rval_ret;
		// assign_method->setSlot_c("base", lval_ret);
		assign_method->setBase(lval_ret);
		ret = assign_method->call(engine, context_chain, 1, tmp);
		// engine->setSignal(INTER_NONE);
		free(tmp);
		CATCH_SIGNAL_RET;

		return ret;
	} else if (lval_ret->address) {
		if (lval_ret->address->setter) { /* if has setter, call it */
			tmp = (Ink_Object **)malloc(sizeof(Ink_Object *));
			tmp[0] = rval_ret;
			// lval_ret->address->setter->setSlot_c("base", lval_ret);
			lval_ret->address->setter->setBase(lval_ret);
			ret = lval_ret->address->setter->call(engine, context_chain, 1, tmp);
			// engine->setSignal(INTER_NONE);
			free(tmp);
			CATCH_SIGNAL_RET;
			return ret;
		} else {
			/* no setter, directly assign */
			lval_ret->address->setValue(rval_ret);
		}
		return is_return_lval ? lval_ret : rval_ret;
	}

	InkWarn_Assigning_Unassignable_Expression(engine);
	//abort();

	RESTORE_LINE_NUM;
	return NULL_OBJ;
}

Ink_Object *Ink_HashTableExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	SET_LINE_NUM;

	Ink_Object *ret = new Ink_Object(engine), *key;
	Ink_Object *hash_method = NULL, **argv = NULL;
	Ink_HashTableMapping::size_type i;

	for (i = 0; i < mapping.size(); i++) {
		/* two possibility: 1. identifier key; 2. expression key with brackets */
		if (mapping[i]->name) {
			ret->setSlot(mapping[i]->name->c_str(), mapping[i]->value->eval(engine, context_chain));
			CATCH_SIGNAL_RET;

		} else {
			key = mapping[i]->key->eval(engine, context_chain);
			CATCH_SIGNAL_RET;

			if (key->type != INK_STRING) {
				InkWarn_Hash_Table_Mapping_Expect_String(engine);
				return NULL_OBJ;
			}
			string tmp = as<Ink_String>(key)->getValue();
			ret->setSlot(tmp.c_str(), mapping[i]->value->eval(engine, context_chain));
			CATCH_SIGNAL_RET;
		}
	}

	hash_method = Ink_IdentifierExpression::getContextSlot(engine, context_chain, "{}", Ink_EvalFlag(), false);
	if (hash_method->type == INK_FUNCTION) {
		argv = (Ink_Object **)malloc(sizeof(Ink_Object *) * 1);
		argv[0] = ret;
		ret = hash_method->call(engine, context_chain, 1, argv);
		free(argv);
	}

	RESTORE_LINE_NUM;
	return ret;
}

Ink_Object *Ink_ListExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	SET_LINE_NUM;

	Ink_ArrayValue val = Ink_ArrayValue();
	Ink_ArrayValue::size_type i;
	Ink_Object *list_method;
	Ink_Object **argv = NULL;
	Ink_Object *ret = NULL;

	for (i = 0; i < elem_list.size(); i++) {
		val.push_back(new Ink_HashTable("", elem_list[i]->eval(engine, context_chain)));
		if (INTER_SIGNAL_RECEIVED) {
			RESTORE_LINE_NUM;
			Ink_Array::disposeArrayValue(val);
			return engine->getInterruptValue();
		}
	}

	ret = new Ink_Array(engine, val);

	list_method = Ink_IdentifierExpression::getContextSlot(engine, context_chain, "[]", Ink_EvalFlag(), false);
	if (list_method->type == INK_FUNCTION) {
		argv = (Ink_Object **)malloc(sizeof(Ink_Object *) * 1);
		argv[0] = ret;
		ret = list_method->call(engine, context_chain, 1, argv);
		free(argv);
	}

	RESTORE_LINE_NUM;
	return ret;
}

Ink_Object *Ink_HashExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	SET_LINE_NUM;

	Ink_Object *base_obj = base->eval(engine, context_chain);
	CATCH_SIGNAL_RET;

	RESTORE_LINE_NUM;
	return getSlot(engine, context_chain, base_obj, slot_id->c_str(), flags);
}

Ink_Object *Ink_HashExpression::getSlot(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain,
										Ink_Object *obj, const char *id, Ink_EvalFlag flags)
{
	Ink_HashTable *hash, *address;
	Ink_Object *base = obj, *ret = NULL, *tmp;
	Ink_Object **argv;
	bool is_from_proto = false;

	if (!(hash = obj->getSlotMapping(engine, id, &is_from_proto)) /* cannot find slot in the origin object */) {
		if (obj->type == INK_UNDEFINED) {
			InkWarn_Get_Slot_Of_Undefined(engine, id);
		}
		address = obj->setSlot(id, NULL);

		if ((tmp = obj->getSlot(engine, "missing"))->type == INK_FUNCTION) {
			/* has missing method, call it */
			// tmp->setSlot_c("base", obj);
			tmp->setBase(obj);
			argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
			argv[0] = new Ink_String(engine, string(id));
			ret = tmp->call(engine, context_chain, 1, argv);
			free(argv);
			goto END;
		} else {
			/* return undefined */
			ret = UNDEFINED;
		}
	} else {
		/* found slot correctly */
		ret = hash->getValue();

		if (!ret) { /* just a placeholder */
			ret = UNDEFINED;
			// assert(!is_from_proto);
		}

		if (is_from_proto) {
			ret = ret->clone(engine);
			address = obj->setSlot(id, NULL);
		} else {
			address = hash;
		}
	}

	/* set address for possible assignment */
	ret->address = address;
	/* set base */
	// ret->setSlot_c("base", base);
	ret->setBase(base);
	ret->setDebugName(id);

	/* call getter if has one */
	if (!flags.is_left_value && hash && hash->getter) {
		// hash->getter->setSlot_c("base", hash->getValue());
		hash->getter->setBase(base);
		ret = hash->getter->call(engine, context_chain, 0, NULL);
		// /* trap all interrupt signal */
		// engine->setSignal(INTER_NONE);
		if (INTER_SIGNAL_RECEIVED) {
			ret = engine->getInterruptValue();
		}
	}

END:

	return ret;
}

Ink_Object *Ink_FunctionExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	SET_LINE_NUM;
	Ink_Protocol protocol;
	Ink_FunctionObject *ret;

	if (protocol_name && (protocol = engine->findProtocol(protocol_name->c_str()))) {
		ret = protocol(engine, param, exp_list, is_macro ? NULL : context_chain->copyContextChain());
	} else {
		ret = new Ink_FunctionObject(engine, param, exp_list,
									 is_macro ? NULL : context_chain->copyContextChain(),
									 is_inline || is_macro);
	}

	if (func_attr) {
		ret->setAttr(*func_attr);
	}

	RESTORE_LINE_NUM;
	return ret;
}

/* expand argument -- expand array object to parameter */
inline Ink_ArgumentList expandArgument(Ink_InterpreteEngine *engine, Ink_Object *obj)
{
	Ink_ArgumentList ret = Ink_ArgumentList();
	Ink_ArrayValue arr_val;
	Ink_ArrayValue::size_type i;

	if (!obj || obj->type != INK_ARRAY) {
		InkWarn_With_Attachment_Require(engine);
		return ret;
	}

	arr_val = as<Ink_Array>(obj)->value;

	for (i = 0; i < arr_val.size(); i++) {
		ret.push_back(new Ink_Argument(new Ink_ShellExpression(arr_val[i] ? arr_val[i]->getValue() : UNDEFINED)));
	}
	return ret;
}

Ink_Object *Ink_CallExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	SET_LINE_NUM;

	Ink_ArgumentList::size_type i;
	Ink_Object **argv = NULL;
	Ink_Object *ret_val, *expandee;
	/* eval callee to get parameter declaration */
	Ink_Object *func = callee->eval(engine, context_chain);
	CATCH_SIGNAL_RET;

	Ink_ParamList param_list = Ink_ParamList();
	Ink_ArgumentList dispose_list, tmp_arg_list, another_tmp_arg_list;
	Ink_ArgcType argc;

	if (func->type == INK_FUNCTION) {
		param_list = as<Ink_FunctionObject>(func)->param;
	}
	if (is_new) {
		func = Ink_HashExpression::getSlot(engine, context_chain, func, "new");
	}

	Ink_Object *base_back = func->getBase();

	tmp_arg_list = Ink_ArgumentList();
	for (Ink_ArgumentList::iterator arg_list_iter = arg_list.begin();
		 arg_list_iter != arg_list.end(); arg_list_iter++) {
		if ((*arg_list_iter)->is_expand) {
			/* if the argument is 'with' argument attachment */

			/* eval expandee */
			expandee = (*arg_list_iter)->expandee->eval(engine, context_chain);
			if (INTER_SIGNAL_RECEIVED) {
				ret_val = engine->getInterruptValue();
				goto DISPOSE_LIST;
			}

			/* expand argument */
			another_tmp_arg_list = expandArgument(engine, expandee);

			/* insert expanded argument to dispose list and temporary argument list */
			dispose_list.insert(dispose_list.end(), another_tmp_arg_list.begin(), another_tmp_arg_list.end());
			tmp_arg_list.insert(tmp_arg_list.end(), another_tmp_arg_list.begin(), another_tmp_arg_list.end());
		} else {
			/* normal argument, directly push to argument list */
			tmp_arg_list.push_back(*arg_list_iter);
		}
	}

	if (tmp_arg_list.size()) {
		/* allocate argument list */
		argv = (Ink_Object **)malloc(tmp_arg_list.size() * sizeof(Ink_Object *));

		/* set argument list, according to the parameter declaration */
		for (i = 0; i < tmp_arg_list.size(); i++) {
			if (i < param_list.size() && param_list[i].is_ref) {
				/* if the parameter is declared as reference, seal the expression to a anonymous function */
				if (param_list[i].is_variant) {
					for (; i < tmp_arg_list.size(); i++) {
						/* if (tmp_arg_list[i]->arg->is_unknown) {
							argv[i] = new Ink_Unknown(engine);
						} else { */
						Ink_ExpressionList exp_list = Ink_ExpressionList();
						exp_list.push_back(tmp_arg_list[i]->arg);
						argv[i] = new Ink_FunctionObject(engine, Ink_ParamList(), exp_list,
														 context_chain->copyContextChain(),
														 true /* is_inline */, true /* is_ref */);
						//}
					}
				} else {
					/* if (tmp_arg_list[i]->arg->is_unknown) {
						argv[i] = new Ink_Unknown(engine);
					} else { */
					Ink_ExpressionList exp_list = Ink_ExpressionList();
					exp_list.push_back(tmp_arg_list[i]->arg);
					argv[i] = new Ink_FunctionObject(engine, Ink_ParamList(), exp_list,
													 context_chain->copyContextChain(),
													 true /* is_inline */, true /* is_ref */);
					// }
				}
			} else {
				/* normal argument */
				argv[i] = tmp_arg_list[i]->arg->eval(engine, context_chain);
				if (INTER_SIGNAL_RECEIVED) {
					ret_val = engine->getInterruptValue();
					/* goto dispose and interrupt */
					goto DISPOSE_ARGV;
				}
			}
		}
	}

	argc = tmp_arg_list.size();
	func->setBase(base_back);

	ret_val = func->call(engine, context_chain, argc, argv);

DISPOSE_ARGV:
	free(argv);

DISPOSE_LIST:
	for (i = 0; i < dispose_list.size(); i++) {
		delete dispose_list[i];
	}

	RESTORE_LINE_NUM;

	return ret_val;
}

Ink_Object *Ink_IdentifierExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	SET_LINE_NUM;

	Ink_Object *ret;
	ret = getContextSlot(engine, context_chain, id->c_str(), flags, if_create_slot);

	RESTORE_LINE_NUM;
	return ret;
}

Ink_Object *Ink_IdentifierExpression::getContextSlot(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, const char *name,
													 Ink_EvalFlag flags, bool if_create_slot)
{
	/* Variables */
	Ink_HashTable *hash, *missing;
	Ink_ContextChain *local = context_chain->getLocal();
	// Ink_ContextChain *global = context_chain->getGlobal();
	Ink_ContextChain *dest_context = local,
					 *base_context = NULL, *missing_base_context = NULL;
	Ink_Object *ret;
	Ink_Object **argv;
	wchar_t *tmp_wstr = NULL;

	hash = context_chain->searchSlotMapping(engine, name, &base_context);
	missing = context_chain->searchSlotMapping(engine, "missing", &missing_base_context);

	/* if the slot cannot be found */
	if (!hash) {
		/* find constant */
		if ((ret = engine->findConstant(wstring(tmp_wstr = Ink_mbstowcs_alloc(name))))
			== NULL) {
			if (if_create_slot) { /* if has the "var" keyword */
				ret = new Ink_Object(engine);
				hash = dest_context->context->setSlot(name, ret);
			} else { /* generate a undefined value */
				if (missing && missing->getValue()->type == INK_FUNCTION) {
					argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
					argv[0] = new Ink_String(engine, name);
					
					if (missing_base_context)
						missing->getValue()->setBase(missing_base_context->context);
					else missing->getValue()->setBase(NULL);

					ret = missing->getValue()->call(engine, context_chain, 1, argv);
					free(argv);
					goto END;
				} else {
					ret = UNDEFINED;
				}
				hash = dest_context->context->setSlot(name, NULL);
			}
		}
	} else {
		ret = hash->getValue(); /* get value */
		if (!ret) { // just a place holder
			ret = UNDEFINED;
		}
	}
	ret->address = hash; /* set its address for assigning */

#if 0
	if (base_context)
		ret->setBase(base_context->context);
	else
#endif
	ret->setBase(NULL);
	
	ret->setDebugName(name);

	/* if it's not a left value reference(which will call setter in assign exp) and has getter, call it */
	if (!flags.is_left_value && hash && hash->getter) {
		if (base_context)
			// hash->getter->setSlot_c("base", base_context->context);
			hash->getter->setBase(base_context->context);
		ret = hash->getter->call(engine, context_chain, 0, NULL);

		// /* trap all interrupt signal */
		// engine->setSignal(INTER_NONE);
		if (INTER_SIGNAL_RECEIVED) {
			ret = engine->getInterruptValue();
		}
		goto END;
	}
	// hash->value->setSlot_c("this", hash->value);

END:

	free(tmp_wstr);

	return ret;
}

Ink_Object *Ink_ArrayLiteral::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	const char *file_name_back;
	Ink_LineNoType line_num_back;
	SET_LINE_NUM;
	Ink_ArrayValue val = Ink_ArrayValue();
	Ink_ArrayValue::size_type i;

	for (i = 0; i < elem_list.size(); i++) {
		val.push_back(new Ink_HashTable("", elem_list[i]->eval(engine, context_chain)));
		if (INTER_SIGNAL_RECEIVED) {
			RESTORE_LINE_NUM;
			Ink_Array::disposeArrayValue(val);
			return engine->getInterruptValue();
		}
	}

	RESTORE_LINE_NUM;
	return new Ink_Array(engine, val);
}

Ink_Object *Ink_NullExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	return NULL_OBJ;
}

Ink_Object *Ink_UndefinedExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	return UNDEFINED;
}

Ink_Object *Ink_NumericExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	return new Ink_Numeric(engine, value);
}

Ink_Object *Ink_StringExpression::eval(Ink_InterpreteEngine *engine, Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	return new Ink_String(engine, *value);
}

}
