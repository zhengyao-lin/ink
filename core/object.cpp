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

extern int inkerr_current_line_number;

Ink_Object *getMethod(Ink_Object *obj, const char *name, InkNative_MethodTable *table, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (!strcmp(name, table[i].name)) {
			return table[i].func->clone();
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

	/* search native methods */
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

	/* no specific method for type, try searching object */
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
/*
Ink_Object *CGC_yield_value = NULL;
ucontext CGC_yield_from;
Ink_Object *CGC_send_back_value = NULL;
ucontext CGC_yield_to;

Ink_Object *InkNative_Generator_Send(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base"), *ret;
	Ink_UContext *uc = as<Ink_UContext>(base->getSlot("context"));
	ucontext *tmp;

	if ((tmp = uc->getContext()) != NULL) {
		CGC_send_back_value = argc ? argv[0] : new Ink_NullObject();
		swapcontext(&CGC_yield_to, tmp);
		uc->setContext(CGC_yield_from);
		return CGC_yield_value;
	} else {
		CGC_yield_value = NULL;
		getcontext(&CGC_yield_to);
		if (CGC_yield_value) {
			uc->setContext(CGC_yield_from);
			return CGC_yield_value;
		}
		ret = base->getSlot("origin")->call(context, argc, argv, this_p);
	}
	return ret;
}
*/
Ink_Object *CGC_interrupt_value = NULL;
// ucontext *CGC_interrupt_address = NULL;

inline Ink_FunctionAttribution getFuncAttr(Ink_Object *obj)
{
	return as<Ink_FunctionObject>(obj)->attr;
}

inline void setFuncAttr(Ink_Object *obj, Ink_FunctionAttribution attr)
{
	as<Ink_FunctionObject>(obj)->setAttr(attr);
	return;
}

inline Ink_Object *callWithAttr(Ink_Object *obj, Ink_FunctionAttribution attr, Ink_ContextChain *context,
								unsigned int argc = 0, Ink_Object **argv = NULL)
{
	Ink_FunctionAttribution attr_back;
	Ink_Object *ret = new Ink_NullObject();

	if (obj->type == INK_FUNCTION) {
		attr_back = getFuncAttr(obj);
		setFuncAttr(obj, attr);
		ret = obj->call(context, argc, argv);
		setFuncAttr(obj, attr_back);
	}
	return ret;
}

#define CURRENT_COROUTINE (ink_coroutine_list[ink_current_coroutine])
#define CURRENT_SLICE (CURRENT_COROUTINE[CURRENT_COROUTINE.size() - 1])
#define CURRENT_CONTEXT (CURRENT_SLICE.context)
#define CURRENT_EXP_LIST (CURRENT_SLICE.exp_list)
#define CURRENT_PC (CURRENT_SLICE.current_pc)
#define CURRENT_GC_ENGINE (CURRENT_SLICE.engine)
#define CURRENT_ATTR (CURRENT_SLICE.func->attr)

#define SWITCH_COROUTINE() (ink_current_coroutine = (ink_current_coroutine + 1 >= ink_coroutine_list.size()) ? 0 : ++ink_current_coroutine)

Ink_CoroutineList ink_coroutine_list;
unsigned int ink_current_coroutine;

extern IGC_CollectEngine *ink_sync_call_tmp_engine;

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
	const char *debug_name_back = getDebugName();
	const char *base_debug_name_back = getSlot("base")->getDebugName();

	/*if (is_generator) {
		Ink_Object *gen = new Ink_Object();
		Ink_FunctionObject *clone_origin = as<Ink_FunctionObject>(this->clone());
		clone_origin->is_generator = false;

		gen->setSlot("origin", clone_origin);
		gen->setSlot("context", new Ink_UContext());
		gen->setSlot("send", new Ink_FunctionObject(InkNative_Generator_Send));

		return gen;
	}*/

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
	}

	for (argi = 0; argi < argc; argi++) {
		if (isUnknown(argv[argi])) { /* find unknown argument */
			tmp = cloneWithPA(argc, copyArgv(argc, argv), true);
			if (if_delete_argv)
				free(argv);
			return tmp;
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

	// reset debug name
	getSlot("base")->setDebugName(base_debug_name_back);
	setDebugName(debug_name_back);

	context->addContext(local);

	/* set trace(unsed for mark&sweep GC) and set debug info */
	Ink_getCurrentEngine()->addTrace(local)->setDebug(inkerr_current_line_number, this);

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
				InterruptSignal signal_backup = CGC_interrupt_signal;
				Ink_Object *value_backup
							= local->ret_val /* set return value of context object for GC to mark */
							= CGC_interrupt_value;

				tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
				CGC_interrupt_signal = INTER_NONE;
				CGC_interrupt_value = NULL;
				switch (signal_backup) {
					case INTER_RETURN:
						if ((tmp = getSlot("retn"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), context, 1, tmp_argv);
						} break;
					case INTER_CONTINUE:
						if ((tmp = getSlot("continue"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), context, 1, tmp_argv);
						} break;
					case INTER_BREAK:
						if ((tmp = getSlot("break"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), context, 1, tmp_argv);
						} break;
					case INTER_DROP:
						if ((tmp = getSlot("drop"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), context, 1, tmp_argv);
						} break;
					case INTER_THROW:
						if ((tmp = getSlot("throw"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), context, 1, tmp_argv);
						} break;
					default: ;
				}
				free(tmp_argv);
				/* restore signal if it hasn't been changed */
				if (CGC_interrupt_signal == INTER_NONE) {
					CGC_interrupt_signal = signal_backup;
					CGC_interrupt_value = value_backup;
				}

				/* whether trap the signal */
				if (attr.hasTrap(CGC_interrupt_signal)) {
					ret_val = trapSignal();
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
		ret_val = local->getSlot("this");
	}

	if (if_delete_argv)
		free(argv);

	/* remove local context from chain and trace */
	context->removeLast();
	Ink_getCurrentEngine()->removeTrace(local);
	
	/* mark return value before sweeping */
	if (ret_val)
		gc_engine->doMark(ret_val);

	gc_engine->collectGarbage();

	/* dispose closure context created */
	if (closure_context) Ink_ContextChain::disposeContextChain(context);

	/* link remaining objects to previous GC engine */
	if (ink_sync_call_tmp_engine) ink_sync_call_tmp_engine->link(gc_engine);
	else if (engine_backup) {
		engine_backup->link(gc_engine);
	}

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



//////////////////////////// Experimental //////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#if 0
inline Ink_CoroutineSlice generateSlice(Ink_ContextChain *context,
										Ink_CoCall sync_call)
{
	Ink_ParamList param;
	unsigned int i, argi;
	Ink_ContextObject *local;
	Ink_Array *var_arg;

	if (sync_call.func->is_native) {
		// TODO: error: native function doesn't sInk_getCurrentEngine()->removeLastTrace();upport coroutine
	}
	/* init GC engine */
	IGC_CollectEngine *gc_engine = new IGC_CollectEngine();
	IGC_initGC(gc_engine);

	/* create new local context */
	local = new Ink_ContextObject();
	if (sync_call.func->closure_context)
		context = sync_call.func->closure_context->copyContextChain();

	if (!sync_call.func->is_inline) { /* if not inline function, set local context */
		local->setSlot("base", sync_call.func->getSlot("base"));
		local->setSlot("this", sync_call.func);
	}
	context->addContext(local);

	/* set trace(unsed for mark&sweep GC) and set debug info */
	Ink_getCurrentEngine()->addTrace(local)->setDebug(inkerr_current_line_number, sync_call.func);

	param = sync_call.func->param;
	unsigned int argc = sync_call.argc;
	Ink_Object **argv = sync_call.argv;
	/* create local variable according to the parameter list */
	for (i = 0, argi = 0; i < param.size(); i++, argi++) {
		if (param[i].is_variant) { /* find variant argument -- break the loop */
			break;
		}
		local->setSlot(param[i].name->c_str(),
					   argi < argc ? argv[argi]
					   			   : new Ink_Undefined()); // initiate local argument
	}

	if (i < param.size() && param[i].is_variant) {
		/* breaked from finding variant arguments */

		/* create variant arguments */
		var_arg = new Ink_Array();
		for (; argi < argc; argi++) {
			/* push arguments in to VA array */
			var_arg->value.push_back(new Ink_HashTable("", argv[argi]));
		}

		/* set VA array */
		local->setSlot(param[i].name->c_str(), var_arg);
	}

	if (argi > argc) { /* still some parameter remaining */
		InkWarn_Unfit_Argument();
	}

	return Ink_CoroutineSlice(sync_call.func, context, gc_engine,
							  sync_call.func->exp_list, 0);
}

inline IGC_CollectEngine *popCurrentSlice()
{
	Ink_getCurrentEngine()->removeTrace(CURRENT_SLICE.context->getLocal());
	Ink_ContextChain::disposeContextChain(CURRENT_SLICE.context);
	IGC_CollectEngine *tmp_eng;
	(tmp_eng = CURRENT_SLICE.engine)->collectGarbage();
	
	CURRENT_COROUTINE.pop_back();
	if (!CURRENT_COROUTINE.size()) {
		ink_coroutine_list.erase(ink_coroutine_list.begin() + ink_current_coroutine);
		if (ink_current_coroutine >= ink_coroutine_list.size()) {
			ink_current_coroutine = 0;
		}
	}
	if (ink_coroutine_list.size() && CURRENT_GC_ENGINE) {
		CURRENT_GC_ENGINE->link(tmp_eng);
		delete tmp_eng;
		return NULL;
	}

	return tmp_eng;
}
#endif

IGC_CollectEngine *ink_sync_call_tmp_engine = NULL;
pthread_mutex_t ink_sync_call_mutex = PTHREAD_MUTEX_INITIALIZER;
int ink_sync_call_max_thread;
int ink_sync_call_current_thread;
int ink_sync_call_ended;
vector<bool> ink_sync_call_end_flag;

class InkCoCall_Argument {
public:
	Ink_ContextChain *context;
	Ink_CoCall sync_call;
	int id;

	InkCoCall_Argument(Ink_ContextChain *context, Ink_CoCall sync_call, int id)
	: context(context), sync_call(sync_call), id(id)
	{ }
};

bool /* if found idling coroutine */ InkCoCall_switchCoroutine()
{
	int id = ink_sync_call_current_thread + 1;
	while (id < ink_sync_call_max_thread) {
		if (!ink_sync_call_end_flag[id]) {
			ink_sync_call_current_thread = id;
			return true;
		}
		id++;
	}
	id = 0;
	while (id < ink_sync_call_current_thread) {
		if (!ink_sync_call_end_flag[id]) {
			ink_sync_call_current_thread = id;
			return true;
		}
		id++;
	}
	return false;
}

void *InkCoCall_primaryCall(void *arg)
{
	InkCoCall_Argument *tmp = (InkCoCall_Argument *)arg;
	int self_id = registerThread(tmp->id);
	unsigned int self_layer = getCurrentLayer();
	printf("***Coroutine created: id %d at layer %u\n", self_id, self_layer);

REWAIT:
	do {
		while (ink_sync_call_current_thread != self_id) ;
	} while (getCurrentLayer() != self_layer);
	if (ink_sync_call_current_thread != self_id) goto REWAIT;

	Ink_Object *ret_val = tmp->sync_call.func->call(tmp->context, tmp->sync_call.argc,
													tmp->sync_call.argv);

	pthread_mutex_lock(&ink_sync_call_mutex);
	// removeThread();
	InkCoCall_switchCoroutine();
	ink_sync_call_ended++;
	ink_sync_call_end_flag[self_id] = true;
	pthread_mutex_unlock(&ink_sync_call_mutex);

	printf("***Coroutine ended: id %d at layer %u\n", self_id, self_layer);

	return ret_val;
}

Ink_Object *InkCoCall_call(Ink_ContextChain *context,
					   Ink_CoCallList call_list)
{
	pthread_t *thread_pool;
	unsigned int i;
	InkCoCall_Argument *tmp;
	vector<InkCoCall_Argument *> dispose_list;
	Ink_Object *ret_val;
	Ink_ArrayValue arr_val;

	printf("***Scheduler Started: %lu coroutines are going to be created\n", call_list.size());
	addLayer();

	int ink_sync_call_max_thread_back = ink_sync_call_max_thread;
	int ink_sync_call_current_thread_back = ink_sync_call_current_thread;
	int ink_sync_call_ended_back = ink_sync_call_ended;
	IGC_CollectEngine *ink_sync_call_tmp_engine_back = ink_sync_call_tmp_engine;
	vector<bool> ink_sync_call_end_flag_back = ink_sync_call_end_flag;
	
	IGC_CollectEngine *engine_backup = Ink_getCurrentEngine()->getCurrentGC();
	
	thread_pool = (pthread_t *)malloc(sizeof(pthread_t) * call_list.size());

	pthread_mutex_lock(&ink_sync_call_mutex);
	ink_sync_call_tmp_engine = engine_backup;
	ink_sync_call_current_thread = -1;
	ink_sync_call_ended = 0;
	ink_sync_call_max_thread = call_list.size();
	ink_sync_call_end_flag = vector<bool>(call_list.size(), false);
	pthread_mutex_unlock(&ink_sync_call_mutex);

	for (i = 0; i < call_list.size(); i++) {
		tmp = new InkCoCall_Argument(context, call_list[i], i);
		pthread_create(&thread_pool[i], NULL, InkCoCall_primaryCall, tmp);
		dispose_list.push_back(tmp);
	}

	pthread_mutex_lock(&ink_sync_call_mutex);
	ink_sync_call_current_thread = 0;
	pthread_mutex_unlock(&ink_sync_call_mutex);
	
	for (i = 0; i < call_list.size(); i++) {
		pthread_join(thread_pool[i], (void **)&ret_val);
		if (ret_val) {
			arr_val.push_back(new Ink_HashTable(ret_val));
		}
	}
	printf("***Scheduler: All coroutine ended. Existing.\n");
	for (i = 0; i < dispose_list.size(); i++) {
		delete dispose_list[i];
	}
	free(thread_pool);
	IGC_initGC(engine_backup);

	pthread_mutex_lock(&ink_sync_call_mutex);
	ink_sync_call_tmp_engine = ink_sync_call_tmp_engine_back;
	ink_sync_call_max_thread = ink_sync_call_max_thread_back;
	ink_sync_call_current_thread = ink_sync_call_current_thread_back;
	ink_sync_call_ended = ink_sync_call_ended_back;
	ink_sync_call_end_flag = ink_sync_call_end_flag_back;
	pthread_mutex_unlock(&ink_sync_call_mutex);

	removeLayer();

	return new Ink_Array(arr_val);
}

/*
Ink_Object *Ink_callSync(Ink_ContextChain *context,
						 Ink_CoCallList call_list)
{
	Ink_CoroutineList ink_coroutine_list_back = ink_coroutine_list;
	unsigned int ink_current_coroutine_back = ink_current_coroutine;
	unsigned int i;
	IGC_CollectEngine *tmp_eng;
	IGC_CollectEngine *engine_backup = Ink_getCurrentEngine()->getCurrentGC();

	ink_coroutine_list = Ink_CoroutineList();
	for (i = 0; i < call_list.size(); i++) {
		ink_coroutine_list.push_back(Ink_Coroutine(1, generateSlice(context, call_list[i])));
	}
	ink_current_coroutine = 0;
	IGC_initGC(CURRENT_GC_ENGINE);

CONTINUE:
	for (; CURRENT_PC < CURRENT_EXP_LIST.size();) {
		CURRENT_GC_ENGINE->checkGC();
		CURRENT_EXP_LIST[CURRENT_PC]->eval(CURRENT_CONTEXT);

		if (CGC_interrupt_signal != INTER_NONE) {
			if (CGC_interrupt_signal == INTER_YIELD) {
				CURRENT_PC++;
				SWITCH_COROUTINE();
				IGC_initGC(CURRENT_GC_ENGINE);
				printf("*** switch to coroutine %d\n", ink_current_coroutine);
				trapSignal();
				continue;
			}
			if (CURRENT_ATTR.hasTrap(CGC_interrupt_signal)) {
				trapSignal();
			}
			break;
		}
		CURRENT_PC++;
	}
	printf("*** coroutine %d:%d ended\n", ink_current_coroutine, CURRENT_COROUTINE.size() - 1);
	tmp_eng = popCurrentSlice();
	if (ink_coroutine_list.size()) {
		IGC_initGC(CURRENT_GC_ENGINE);
		printf("*** continue on coroutine %d\n", ink_current_coroutine);
		goto CONTINUE;
	}
	if (tmp_eng) {
		engine_backup->link(tmp_eng);
		delete tmp_eng;
	}
	IGC_initGC(engine_backup);

	ink_coroutine_list = ink_coroutine_list_back;
	ink_current_coroutine = ink_current_coroutine_back;

	return NULL_OBJ;
}
*/
