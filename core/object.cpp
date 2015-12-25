#include <string.h>
#include "hash.h"
#include "object.h"
#include "expression.h"
#include "gc/collect.h"
#include "native/native.h"
#include "../interface/engine.h"
#include "coroutine/coroutine.h"
#include "native/general.h"

extern int numeric_native_method_table_count;
extern InkNative_MethodTable numeric_native_method_table[];
extern int string_native_method_table_count;
extern InkNative_MethodTable string_native_method_table[];
extern int object_native_method_table_count;
extern InkNative_MethodTable object_native_method_table[];
extern int array_native_method_table_count;
extern InkNative_MethodTable array_native_method_table[];
extern int function_native_method_table_count;
extern InkNative_MethodTable function_native_method_table[];

Ink_Object *getMethod(Ink_Object *obj, const char *name, InkNative_MethodTable *table, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (!strcmp(name, table[i].name)) {
			return new Ink_FunctionObject(as<Ink_FunctionObject>(table[i].func)->native);
		}
	}
	return NULL;
}

Ink_Object *Ink_Object::getSlot(const char *key)
{
	Ink_HashTable *ret = getSlotMapping(key);

	return ret ? ret->getValue() : new Ink_Undefined();
}

Ink_HashTable *Ink_Object::getSlotMapping(const char *key)
{
	Ink_HashTable *i;
	Ink_HashTable *ret = NULL;
	Ink_Object *method = NULL;

	for (i = hash_table; i; i = i->next) {
		if (!strcmp(i->key, key) && (i->getValue() || i->bonding)) {
			for (ret = i; ret->bonding; ret = ret->bonding) ;
			ret->bondee = i;
			return ret;
		}
	}

	switch (type) {
		case INK_NUMERIC:
			method = getMethod(this, key, numeric_native_method_table,
							   numeric_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		case INK_STRING:
			method = getMethod(this, key, string_native_method_table,
							   string_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		case INK_ARRAY:
			method = getMethod(this, key, array_native_method_table,
							   array_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		case INK_FUNCTION:
			method = getMethod(this, key, function_native_method_table,
							   function_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		default: break;
	}
	if (!ret) {
		method = getMethod(this, key, object_native_method_table,
						   object_native_method_table_count);
		if (method)
		ret = setSlot(key, method, false);
	}

	return ret;
}

Ink_HashTable *Ink_Object::setSlot(const char *key, Ink_Object *value, bool if_check_exist)
{
	Ink_HashTable *slot = NULL;

	if (if_check_exist) slot = getSlotMapping(key);

	if (slot) {
		slot->setValue(value);
	} else {
		slot = new Ink_HashTable(key, value);
		if (hash_table)
			hash_table->getEnd()->next = slot;
		else
			hash_table = slot;
	}

	return slot;
}

void Ink_Object::deleteSlot(const char *key)
{
	Ink_HashTable *i, *prev;

	for (i = hash_table, prev = NULL; i; prev = i, i = i->next) {
		if (!strcmp(i->key, key)) {
			if (prev) {
				prev->next = i->next;
			} else {
				hash_table = i->next;
			}
			delete i;
			return;
		}
	}

	return;
}

void Ink_Object::cleanHashTable()
{
	cleanHashTable(hash_table);
	hash_table = NULL;

	return;
}

void Ink_Object::cleanHashTable(Ink_HashTable *table)
{
	Ink_HashTable *i, *tmp;
	for (i = table; i;) {
		tmp = i;
		i = i->next;
		delete tmp;
	}

	return;
}

void Ink_Object::cloneHashTable(Ink_Object *src, Ink_Object *dest)
{
	Ink_HashTable *i;
	for (i = src->hash_table; i; i = i->next) {
		if (i->getValue())
			dest->setSlot(i->key, i->getValue());
	}

	return;
}

Ink_Object *Ink_Object::clone()
{
	Ink_Object *new_obj = new Ink_Object();

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Numeric::clone()
{
	Ink_Object *new_obj = new Ink_Numeric(value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_String::clone()
{
	Ink_Object *new_obj = new Ink_String(value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *InkNative_Generator_Send(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	return NULL;
}

inline Ink_Object **copyArgv(int argc, Ink_Object **argv)
{
	Ink_Object **ret = (Ink_Object **)malloc(sizeof(Ink_Object *) * argc);
	memcpy(ret, argv, sizeof(Ink_Object *) * argc);
	return ret;
}

inline Ink_Object **linkArgv(int argc1, Ink_Object **argv1, int argc2, Ink_Object **argv2)
{
	Ink_Object **ret = (Ink_Object **)malloc(sizeof(Ink_Object *) * (argc1 + argc2));
	memcpy(ret, argv1, sizeof(Ink_Object *) * argc1);
	memcpy(&ret[argc1], argv2, sizeof(Ink_Object *) * argc2);
	return ret;
}

Ink_Object *Ink_FunctionObject::call(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv,
									 Ink_Object *this_p, bool if_return_this)
{
	unsigned int argi, j, tmp_argc;
	Ink_ContextObject *local;
	Ink_Object *ret_val = NULL;
	Ink_Array *var_arg = NULL;
	IGC_CollectEngine *engine_backup = Ink_getCurrentEngine()->getCurrentGC();
	Ink_Object *tmp = NULL;
	Ink_Object **tmp_argv = NULL;
	bool force_return = false;
	bool if_delete_argv = false;

	/*
	if (is_generator) {
		Ink_Object *gen = new Ink_Object();
		Ink_FunctionObject *clone_origin = as<Ink_FunctionObject>(this->clone());
		clone_origin->is_generator = false;

		gen->setSlot("origin", clone_origin);
		gen->setSlot("address", new Ink_Array());
		gen->setSlot("send", new Ink_FunctionObject(InkNative_Generator_Send));

		return gen;
	}
	*/
	bool is_arg_completed = true;

	/* if some arguments have been applied already */
	if (partial_applied_argv) {
		tmp_argc = partial_applied_argc;
		tmp_argv = copyArgv(partial_applied_argc, partial_applied_argv);
		for (j = 0, argi = 0; j < tmp_argc; j++) {
			/* find unknown place to put in arguments */
			if (isUnknown(tmp_argv[j])) {
				if (argi < argc /* not excess */
					&& !isUnknown(argv[argi]) /* not another unknown argument */)
					tmp_argv[j] = argv[argi];
				else
					is_arg_completed = false;
				argi++;
			}
		}

		if (!is_arg_completed) {
			/* still missing arguments -- return another PAF */
			if (argi < argc) {
				unsigned int remainc = argc - argi; /* remaining arguments */
				argc = remainc + tmp_argc;
				/* link the PA arguments and remaining arguments */
				argv = linkArgv(tmp_argc, tmp_argv,
								remainc, &argv[argi]);

				free(tmp_argv);
				tmp_argc = argc;
				tmp_argv = argv;
			}
			return cloneWithPA(tmp_argc, tmp_argv, true);
		}

		unsigned int remainc = argc - argi; /* remaining arguments */
		argc = remainc + tmp_argc;
		/* link the PA arguments and remaining arguments */
		argv = linkArgv(tmp_argc, tmp_argv,
						remainc, &argv[argi]);
		free(tmp_argv);
		if_delete_argv = true;
	} else {
		for (argi = 0; argi < argc; argi++) {
			if (isUnknown(argv[argi])) { /* find unknown argument */
				return cloneWithPA(argc, copyArgv(argc, argv), true);
			}
		}
	}

	/* init GC engine */
	IGC_CollectEngine *gc_engine = new IGC_CollectEngine();
	IGC_initGC(gc_engine);

	/* create new local context */
	local = new Ink_ContextObject();
	if (closure_context)
		context = closure_context->copyContextChain(); /* copy closure context chain */

	if (!is_inline) { /* if not inline function, set local context */
		local->setSlot("base", getSlot("base"));
		local->setSlot("this", this);
	}

	/* set "this" pointer if exists */
	if (this_p)
		local->setSlot("this", this_p);

	context->addContext(local);

	/* set trace(unsed for mark&sweep GC) */
	Ink_getCurrentEngine()->addTrace(local);

	/* set local context */
	// gc_engine->initContext(context);

	if (is_native) {
		/* if it's a native function, call the function pointer */
		ret_val = native(context, argc, argv, this_p);
	} else {
		/* create local variable according to the parameter list */
		for (j = 0, argi = 0; j < param.size(); j++, argi++) {
			if (param[j].is_variant) { /* find variant argument -- break the loop */
				break;
			}
			local->setSlot(param[j].name->c_str(),
						   argi < argc ? argv[argi]
						   			   : new Ink_Undefined()); // initiate local argument
		}

		if (j < param.size() && param[j].is_variant) {
			/* breaked from finding variant arguments */

			/* create variant arguments */
			var_arg = new Ink_Array();
			for (; argi < argc; argi++) {
				/* push arguments in to VA array */
				var_arg->value.push_back(new Ink_HashTable("", argv[argi]));
			}

			/* set VA array */
			local->setSlot(param[j].name->c_str(), var_arg);
		}

		if (argi > argc) { /* still some parameter remaining */
			InkWarn_Unfit_Argument();
		}

		for (j = 0; j < exp_list.size(); j++) {
			gc_engine->checkGC();
			ret_val = exp_list[j]->eval(context); // eval each expression

			/* interrupt signal received */
			if (CGC_interrupt_signal != INTER_NONE) {

				/* interrupt event triggered */
				switch (CGC_interrupt_signal) {
					case INTER_RETURN:
						if ((tmp = getSlot("return"))->type == INK_FUNCTION) {
							tmp->call(context);
						} break;
					case INTER_CONTINUE:
						if ((tmp = getSlot("continue"))->type == INK_FUNCTION) {
							tmp->call(context);
						} break;
					case INTER_BREAK:
						if ((tmp = getSlot("break"))->type == INK_FUNCTION) {
							tmp->call(context);
						} break;
					default: ;
				}

				/* whether trap the signal */
				if (attr.hasTrap(CGC_interrupt_signal))
					CGC_interrupt_signal = INTER_NONE;
				force_return = true;
				break;
			}
		}
	}

	/* conditions of returning "this" pointer:
	 * 1. has "this" pointer as argument
	 * 2. bool if_return_this is true(in default)
	 * 3. no force return
	 */
	if (this_p && if_return_this && !force_return) {
		ret_val = local->getSlot("this");
	}

	if (if_delete_argv)
		free(argv);

	/* remove local context from chain and trace */
	context->removeLast();
	Ink_getCurrentEngine()->removeLastTrace();
	
	/* mark return value before sweeping */
	if (ret_val)
		gc_engine->doMark(ret_val);

	gc_engine->collectGarbage();

	/* dispose closure context created */
	if (closure_context) Ink_ContextChain::disposeContextChain(context);

	/* link remaining objects to previous GC engine */
	if (engine_backup) engine_backup->link(gc_engine);

	/* restore GC engine */
	IGC_initGC(engine_backup);
	delete gc_engine;

	return ret_val ? ret_val : new Ink_NullObject(); // return the last expression
}

Ink_ArrayValue Ink_Array::cloneArrayValue(Ink_ArrayValue val)
{
	Ink_ArrayValue ret = Ink_ArrayValue();
	unsigned int i;

	for (i = 0; i < val.size(); i++) {
		if (val[i])
			ret.push_back(new Ink_HashTable("", val[i]->getValue()));
		else ret.push_back(NULL);
	}

	return ret;
}

Ink_Object *Ink_Array::clone()
{
	Ink_Object *new_obj = new Ink_Array(cloneArrayValue(value));

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_FunctionObject::clone()
{
	Ink_FunctionObject *new_obj = new Ink_FunctionObject();

	new_obj->is_native = is_native;
	new_obj->is_inline = is_inline;
	new_obj->is_generator = is_generator;
	new_obj->native = native;

	new_obj->param = param;
	new_obj->exp_list = exp_list;
	if (new_obj->closure_context)
		new_obj->closure_context = closure_context->copyContextChain();
	new_obj->attr = attr;
	new_obj->partial_applied_argc = partial_applied_argc;
	new_obj->partial_applied_argv = copyArgv(partial_applied_argc, partial_applied_argv);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_FunctionObject::~Ink_FunctionObject()
{
	if (closure_context) Ink_ContextChain::disposeContextChain(closure_context);
	if (partial_applied_argv) free(partial_applied_argv);
	cleanHashTable();
}

Ink_Object *Ink_Unknown::clone()
{
	Ink_Object *new_obj = new Ink_Unknown();

	cloneHashTable(this, new_obj);

	return new_obj;
}