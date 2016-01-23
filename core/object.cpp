#include <string.h>
#include "hash.h"
#include "object.h"
#include "expression.h"
#include "gc/collect.h"
#include "native/native.h"
#include "interface/engine.h"
#include "coroutine/coroutine.h"
#include "thread/thread.h"
#include "native/general.h"

using namespace std;

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
extern int big_num_native_method_table_count;
extern InkNative_MethodTable big_num_native_method_table[];

Ink_Object *getMethod(Ink_InterpreteEngine *engine,
					  Ink_Object *obj, const char *name, InkNative_MethodTable *table, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (!strcmp(name, table[i].name)) {
			return table[i].func->clone(engine);
		}
	}
	return NULL;
}

Ink_Object *Ink_Object::getSlot(Ink_InterpreteEngine *engine, const char *key)
{
	Ink_HashTable *ret = getSlotMapping(engine, key);

	return ret ? ret->getValue() : UNDEFINED;
}

Ink_HashTable *Ink_Object::getSlotMapping(Ink_InterpreteEngine *engine, const char *key)
{
	Ink_HashTable *i;
	Ink_HashTable *ret = NULL;
	// Ink_Object *method = NULL;

	for (i = hash_table; i; i = i->next) {
		if (!strcmp(i->key, key) && (i->getValue() || i->bonding)) {
			for (ret = i; ret->bonding; ret = ret->bonding) ;
			ret->bondee = i;
			return ret;
		}
	}
	if (!engine) return ret;

#if 0
	/* search native methods */
	switch (type) {
		case INK_NUMERIC:
			method = getMethod(engine, this, key, numeric_native_method_table,
							   numeric_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		case INK_BIGNUMERIC:
			method = getMethod(engine, this, key, big_num_native_method_table,
							   big_num_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		case INK_STRING:
			method = getMethod(engine, this, key, string_native_method_table,
							   string_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		case INK_ARRAY:
			method = getMethod(engine, this, key, array_native_method_table,
							   array_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		case INK_FUNCTION:
			method = getMethod(engine, this, key, function_native_method_table,
							   function_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		default: break;
	}

	/* no specific method for type, try searching object */
	if (!ret) {
		method = getMethod(engine, this, key, object_native_method_table,
						   object_native_method_table_count);
		if (method)
		ret = setSlot(key, method, false);
	}
#endif
	ret = engine->searchNativeMethod(type, key);
	if (ret)
		ret = setSlot(key, ret->getValue()->cloneDeep(engine));

	return ret;
}

Ink_HashTable *Ink_Object::setSlot(const char *key, Ink_Object *value, bool if_check_exist, string *key_p)
{
	Ink_HashTable *i, *slot = NULL, *bond_tracer;

	if (if_check_exist)
		for (i = hash_table; i; i = i->next) {
			if (!strcmp(i->key, key)) {
				for (bond_tracer = i; bond_tracer->bonding; bond_tracer = bond_tracer->bonding) ;
				bond_tracer->bondee = i;
				slot = bond_tracer;
			}
		}

	if (slot) {
		slot->setValue(value);
		if (key_p) delete key_p;
	} else {
		slot = new Ink_HashTable(key, value, key_p);
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
			dest->setSlot(i->key, i->getValue(), false);
	}

	return;
}

void Ink_Object::cloneDeepHashTable(Ink_InterpreteEngine *engine, Ink_Object *src, Ink_Object *dest)
{
	Ink_HashTable *i;
	for (i = src->hash_table; i; i = i->next) {
		if (i->getValue()
			&& !engine->cloneDeepHasTraced(i->getValue()))
			dest->setSlot(i->key, i->getValue()->cloneDeep(engine), false);
	}

	return;
}

Ink_Object *Ink_Object::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_Object(engine);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Object::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_Object(engine);

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *Ink_ContextObject::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_ContextObject(engine);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_ContextObject::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_ContextObject(engine);

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Numeric::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_Numeric(engine, value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Numeric::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_Numeric(engine, value);

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *Ink_BigNumeric::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_BigNumeric(engine, value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_BigNumeric::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_BigNumeric(engine, value);

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *Ink_String::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_String(engine, getValue());

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_String::cloneDeep(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_String(engine, getValue());

	engine->addDeepCloneTrace(this);
	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

inline Ink_FunctionAttribution getFuncAttr(Ink_Object *obj)
{
	return as<Ink_FunctionObject>(obj)->attr;
}

inline void setFuncAttr(Ink_Object *obj, Ink_FunctionAttribution attr)
{
	as<Ink_FunctionObject>(obj)->setAttr(attr);
	return;
}

inline Ink_Object *callWithAttr(Ink_Object *obj, Ink_FunctionAttribution attr,
								Ink_InterpreteEngine *engine, Ink_ContextChain *context,
								Ink_ArgcType argc = 0, Ink_Object **argv = NULL)
{
	Ink_FunctionAttribution attr_back;
	Ink_Object *ret = NULL_OBJ;

	if (obj->type == INK_FUNCTION) {
		attr_back = getFuncAttr(obj);
		setFuncAttr(obj, attr);
		ret = obj->call(engine, context, argc, argv);
		setFuncAttr(obj, attr_back);
	}
	return ret;
}

inline Ink_Object **copyDeepArgv(Ink_InterpreteEngine *engine,
								 Ink_ArgcType argc, Ink_Object **argv)
{
	Ink_Object **ret = (Ink_Object **)malloc(sizeof(Ink_Object *) * argc);
	Ink_ArgcType i;

	for (i = 0; i < argc; i++) {
		if (!engine->cloneDeepHasTraced(argv[i])) {
			ret[i] = argv[i]->cloneDeep(engine);
		} else {
			ret[i] = UNDEFINED;
		}
	}

	return ret;
}

Ink_Object *Ink_FunctionObject::call(Ink_InterpreteEngine *engine,
									 Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv,
									 Ink_Object *this_p, bool if_return_this)
{
	Ink_ExpressionList::size_type i;
	Ink_ArgcType argi;
	Ink_ParamList::size_type j;
	Ink_ContextObject *local;
	Ink_Object *ret_val = NULL;
	Ink_Array *var_arg = NULL;
	IGC_CollectEngine *gc_engine_backup = engine->getCurrentGC();
	Ink_Object *tmp = NULL;
	Ink_Object **tmp_argv = NULL;
	bool force_return = false;
	bool if_delete_argv = false;
	const char *debug_name_back = getDebugName();
	const char *base_debug_name_back = getSlot(engine, "base")->getDebugName();

	/* init GC engine */
	IGC_CollectEngine *gc_engine = new IGC_CollectEngine(engine);
	engine->setCurrentGC(gc_engine);

	/* create new local context */
	local = new Ink_ContextObject(engine);
	if (closure_context)
		context = closure_context->copyContextChain(); /* copy closure context chain */

	if (!is_inline) { /* if not inline function, set local context */
		local->setSlot("base", getSlot(engine, "base"));
		local->setSlot("this", this);
	}

	/* set "this" pointer if exists */
	if (this_p)
		local->setSlot("this", this_p);

	// reset debug name
	getSlot(engine, "base")->setDebugName(base_debug_name_back);
	setDebugName(debug_name_back);

	context->addContext(local);

	/* set trace(unsed for mark&sweep GC) and set debug info */
	engine->addTrace(local)->setDebug(engine->current_line_number, this);

	/* set local context */
	// gc_engine->initContext(context);

	if (is_native) {
		/* if it's a native function, call the function pointer */
		ret_val = native(engine, context, argc, argv, this_p);
	} else {
		/* create local variable according to the parameter list */
		for (j = 0, argi = 0; j < param.size(); j++, argi++) {
			if (param[j].is_variant) { /* find variant argument -- break the loop */
				break;
			}
			local->setSlot(param[j].name->c_str(),
						   argi < argc ? argv[argi]
						   			   : UNDEFINED); // initiate local argument
		}

		if (j < param.size() && param[j].is_variant) {
			/* breaked from finding variant arguments */

			/* create variant arguments */
			var_arg = new Ink_Array(engine);
			for (; argi < argc; argi++) {
				/* push arguments in to VA array */
				var_arg->value.push_back(new Ink_HashTable("", argv[argi]));
			}

			/* set VA array */
			local->setSlot(param[j].name->c_str(), var_arg);
		}

		if (argi > argc) { /* still some parameter remaining */
			InkWarn_Unfit_Argument(engine);
		}

		for (i = 0; i < exp_list.size(); i++) {
			gc_engine->checkGC();
			ret_val = exp_list[i]->eval(engine, context); // eval each expression

			/* interrupt signal received */
			if (engine->CGC_interrupt_signal != INTER_NONE) {
				/* interrupt event triggered */
				InterruptSignal signal_backup = engine->CGC_interrupt_signal;
				Ink_Object *value_backup
							= local->ret_val /* set return value of context object for GC to mark */
							= engine->CGC_interrupt_value;

				tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
				engine->CGC_interrupt_signal = INTER_NONE;
				engine->CGC_interrupt_value = NULL;
				switch (signal_backup) {
					case INTER_RETURN:
						if ((tmp = getSlot(engine, "retn"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), engine, context, 1, tmp_argv);
						} break;
					case INTER_CONTINUE:
						if ((tmp = getSlot(engine, "continue"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), engine, context, 1, tmp_argv);
						} break;
					case INTER_BREAK:
						if ((tmp = getSlot(engine, "break"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), engine, context, 1, tmp_argv);
						} break;
					case INTER_DROP:
						if ((tmp = getSlot(engine, "drop"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), engine, context, 1, tmp_argv);
						} break;
					case INTER_THROW:
						if ((tmp = getSlot(engine, "throw"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), engine, context, 1, tmp_argv);
						} break;
					default: ;
				}
				free(tmp_argv);
				/* restore signal if it hasn't been changed */
				if (engine->CGC_interrupt_signal == INTER_NONE) {
					engine->CGC_interrupt_signal = signal_backup;
					engine->CGC_interrupt_value = value_backup;
				}

				/* whether trap the signal */
				if (attr.hasTrap(engine->CGC_interrupt_signal)) {
					ret_val = trapSignal(engine);
				} else {
					ret_val = NULL;
				}

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
		ret_val = local->getSlot(engine, "this");
	}

	if (if_delete_argv)
		free(argv);

	/* remove local context from chain and trace */
	context->removeLast();
	engine->removeTrace(local);
	
	/* mark return value before sweeping */
	if (ret_val)
		gc_engine->doMark(ret_val);

	gc_engine->collectGarbage();

	/* dispose closure context created */
	if (closure_context) Ink_ContextChain::disposeContextChain(context);

	/* link remaining objects to previous GC engine */
	if (engine->ink_sync_call_tmp_engine) engine->ink_sync_call_tmp_engine->link(gc_engine);
	else if (gc_engine_backup) {
		gc_engine_backup->link(gc_engine);
	}

	/* restore GC engine */
	engine->setCurrentGC(gc_engine_backup);
	delete gc_engine;

	return ret_val ? ret_val : NULL_OBJ; // return the last expression
}

Ink_ArrayValue Ink_Array::cloneArrayValue(Ink_ArrayValue val)
{
	Ink_ArrayValue ret = Ink_ArrayValue();
	Ink_ArrayValue::size_type i;

	for (i = 0; i < val.size(); i++) {
		if (val[i])
			ret.push_back(new Ink_HashTable("", val[i]->getValue()));
		else ret.push_back(NULL);
	}

	return ret;
}

Ink_ArrayValue Ink_Array::cloneDeepArrayValue(Ink_InterpreteEngine *engine, Ink_ArrayValue val)
{
	Ink_ArrayValue ret = Ink_ArrayValue();
	Ink_ArrayValue::size_type i;

	for (i = 0; i < val.size(); i++) {
		if (val[i]
			&& !engine->cloneDeepHasTraced(val[i]->getValue())) {
			ret.push_back(new Ink_HashTable("", val[i]->getValue()->cloneDeep(engine)));
		}
		else ret.push_back(NULL);
	}

	return ret;
}

Ink_Object *Ink_Array::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_Array(engine, cloneArrayValue(value));

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Array::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_Array(engine, cloneDeepArrayValue(engine, value));

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *Ink_FunctionObject::clone(Ink_InterpreteEngine *engine)
{
	Ink_FunctionObject *new_obj = new Ink_FunctionObject(engine);
	
	new_obj->is_native = is_native;
	new_obj->is_inline = is_inline;
	new_obj->is_generator = is_generator;
	new_obj->native = native;

	new_obj->param = param;
	new_obj->exp_list = exp_list;
	if (closure_context)
		new_obj->closure_context = closure_context->copyContextChain();
	new_obj->attr = attr;
	new_obj->partial_applied_argc = partial_applied_argc;
	new_obj->partial_applied_argv = copyArgv(partial_applied_argc, partial_applied_argv);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_FunctionObject::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_FunctionObject *new_obj = new Ink_FunctionObject(engine);
	
	new_obj->is_native = is_native;
	new_obj->is_inline = is_inline;
	new_obj->is_generator = is_generator;
	new_obj->native = native;

	new_obj->param = param;
	new_obj->exp_list = exp_list;
	if (closure_context) {
		new_obj->closure_context = closure_context->copyDeepContextChain(engine);
	}
	new_obj->attr = attr;
	new_obj->partial_applied_argc = partial_applied_argc;
	new_obj->partial_applied_argv = copyDeepArgv(engine, partial_applied_argc, partial_applied_argv);

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_FunctionObject::~Ink_FunctionObject()
{
	if (closure_context) Ink_ContextChain::disposeContextChain(closure_context);
	if (partial_applied_argv) free(partial_applied_argv);
	cleanHashTable();
}

Ink_Object *Ink_Undefined::cloneDeep(Ink_InterpreteEngine *engine)
{
	return UNDEFINED;
}

Ink_Object *Ink_NullObject::cloneDeep(Ink_InterpreteEngine *engine)
{
	return NULL_OBJ;
}

Ink_Object *Ink_Unknown::cloneDeep(Ink_InterpreteEngine *engine)
{
	return new Ink_Unknown(engine);
}

class InkCoCall_Argument {
public:
	Ink_InterpreteEngine *engine;
	Ink_ContextChain *context;
	Ink_CoCall sync_call;
	ThreadID id;

	InkCoCall_Argument(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_CoCall sync_call, ThreadID id)
	: engine(engine), context(context), sync_call(sync_call), id(id)
	{ }
};

bool /* if found idling coroutine */
InkCoCall_switchCoroutine(Ink_InterpreteEngine *engine)
{
	ThreadID id = engine->ink_sync_call_current_thread + 1;
	while (id < engine->ink_sync_call_max_thread) {
		if (!engine->ink_sync_call_end_flag[id]) {
			engine->ink_sync_call_current_thread = id;
			return true;
		}
		id++;
	}
	id = 1;
	while (id < engine->ink_sync_call_current_thread) {
		if (!engine->ink_sync_call_end_flag[id]) {
			engine->ink_sync_call_current_thread = id;
			return true;
		}
		id++;
	}
	return false;
}

void *InkCoCall_primaryCall(void *arg)
{
	InkCoCall_Argument *tmp = (InkCoCall_Argument *)arg;
	ThreadID self_id = tmp->engine->registerThread(tmp->id);
	ThreadLayerType self_layer = tmp->engine->getCurrentLayer();
	// printf("***Coroutine created: id %d at layer %u\n", self_id, self_layer);

REWAIT:
	do {
		while (1) {
			pthread_mutex_lock(&tmp->engine->ink_sync_call_mutex);
			if (tmp->engine->ink_sync_call_current_thread == self_id) {
				pthread_mutex_unlock(&tmp->engine->ink_sync_call_mutex);
				break;
			}
			pthread_mutex_unlock(&tmp->engine->ink_sync_call_mutex);
		}
	} while (tmp->engine->getCurrentLayer() != self_layer);
	pthread_mutex_lock(&tmp->engine->ink_sync_call_mutex);
	if (tmp->engine->ink_sync_call_current_thread != self_id) {
		pthread_mutex_unlock(&tmp->engine->ink_sync_call_mutex);
		goto REWAIT;
	}
	pthread_mutex_unlock(&tmp->engine->ink_sync_call_mutex);

	Ink_Object *ret_val = tmp->sync_call.func->call(tmp->engine, tmp->context, tmp->sync_call.argc,
													tmp->sync_call.argv);

	pthread_mutex_lock(&tmp->engine->ink_sync_call_mutex);
	// removeThread();
	InkCoCall_switchCoroutine(tmp->engine);
	tmp->engine->ink_sync_call_end_flag[self_id] = true;
	pthread_mutex_unlock(&tmp->engine->ink_sync_call_mutex);

	// printf("***Coroutine ended: id %u at layer %u\n", self_id, self_layer);

	return ret_val;
}

Ink_Object *InkCoCall_call(Ink_InterpreteEngine *engine,
						   Ink_ContextChain *context,
						   Ink_CoCallList call_list)
{
	pthread_t *thread_pool;
	Ink_CoCallList::size_type i;
	ThreadID th_id;
	InkCoCall_Argument *tmp;
	vector<InkCoCall_Argument *> dispose_list;
	vector<InkCoCall_Argument *>::size_type j;
	Ink_Object *ret_val;
	Ink_ArrayValue arr_val;

	// printf("***Scheduler Started: %lu coroutines are going to be created\n", call_list.size());
	engine->addLayer();

	ThreadID ink_sync_call_max_thread_back = engine->ink_sync_call_max_thread;
	ThreadID ink_sync_call_current_thread_back = engine->ink_sync_call_current_thread;
	IGC_CollectEngine *ink_sync_call_tmp_engine_back = engine->ink_sync_call_tmp_engine;
	vector<bool> ink_sync_call_end_flag_back = engine->ink_sync_call_end_flag;

	IGC_CollectEngine *gc_engine_backup = engine->getCurrentGC();
	
	thread_pool = (pthread_t *)malloc(sizeof(pthread_t) * call_list.size());

	pthread_mutex_lock(&engine->ink_sync_call_mutex);
	engine->ink_sync_call_tmp_engine = gc_engine_backup;
	engine->ink_sync_call_current_thread = 0;
	engine->ink_sync_call_max_thread = call_list.size() + 1;
	engine->ink_sync_call_end_flag = vector<bool>(call_list.size() + 1, false);
	pthread_mutex_unlock(&engine->ink_sync_call_mutex);

	for (i = 0, th_id = 1; i < call_list.size(); i++, th_id++) {
		tmp = new InkCoCall_Argument(engine, context, call_list[i], th_id);
		pthread_create(&thread_pool[i], NULL, InkCoCall_primaryCall, tmp);
		dispose_list.push_back(tmp);
	}

	pthread_mutex_lock(&engine->ink_sync_call_mutex);
	engine->ink_sync_call_current_thread = 1;
	pthread_mutex_unlock(&engine->ink_sync_call_mutex);
	
	for (i = 0; i < call_list.size(); i++) {
		pthread_join(thread_pool[i], (void **)&ret_val);
		if (ret_val) {
			arr_val.push_back(new Ink_HashTable(ret_val));
		}
	}
	// printf("***Scheduler: All coroutine ended. Existing.\n");
	for (j = 0; j < dispose_list.size(); j++) {
		delete dispose_list[j];
	}
	free(thread_pool);
	engine->setCurrentGC(gc_engine_backup);

	pthread_mutex_lock(&engine->ink_sync_call_mutex);
	engine->ink_sync_call_tmp_engine = ink_sync_call_tmp_engine_back;
	engine->ink_sync_call_max_thread = ink_sync_call_max_thread_back;
	engine->ink_sync_call_current_thread = ink_sync_call_current_thread_back;
	engine->ink_sync_call_end_flag = ink_sync_call_end_flag_back;
	pthread_mutex_unlock(&engine->ink_sync_call_mutex);

	engine->removeLayer();

	return new Ink_Array(engine, arr_val);
}