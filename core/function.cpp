#include <stdlib.h>
#include "error.h"
#include "object.h"
#include "expression.h"
#include "gc/collect.h"
#include "interface/engine.h"

namespace ink {

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

Ink_Object **Ink_FunctionObject::copyDeepArgv(Ink_InterpreteEngine *engine,
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

void Ink_FunctionObject::triggerInterruptEvent(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
											   Ink_ContextObject *local, Ink_Object *receiver)
{
	Ink_Object *tmp = NULL;
	Ink_Object **tmp_argv = NULL;
	Ink_InterruptSignal signal_backup = engine->getSignal();
	Ink_Object *value_backup
				= local->ret_val /* set return value of context object for GC to mark */
				= engine->getInterruptValue();
	string *tmp_str = NULL;
	bool event_found = false;

	tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
	engine->setInterrupt(INTER_NONE, NULL);
	if (signal_backup < INTER_LAST) {
		if ((tmp = receiver->getSlot(engine, (string("@") + getNativeSignalName(signal_backup)).c_str()))
			->type == INK_FUNCTION) {
			tmp_argv[0] = value_backup;
			event_found = true;
			callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), engine, context, 1, tmp_argv);
		}
	} else if (signal_backup > INTER_LAST) {
		// custom signal
		tmp_str = engine->getCustomInterruptSignalName(signal_backup);
		if (tmp_str) {
			if ((tmp = receiver->getSlot(engine, (string("@") + *tmp_str).c_str()))
				->type == INK_FUNCTION) {
				tmp_argv[0] = value_backup;
				event_found = true;
				callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), engine, context, 1, tmp_argv);
			}
		} else {
			InkWarn_Unregistered_Interrupt_Signal(engine, signal_backup);
		}
	} else {
		InkWarn_Unregistered_Interrupt_Signal(engine, signal_backup);
	}
	free(tmp_argv);

	/* restore signal if no event found */
	if (!event_found) {
		engine->setInterrupt(signal_backup, value_backup);
	}
	
	return;
}

Ink_Object *Ink_FunctionObject::checkUnkownArgument(Ink_ArgcType &argc, Ink_Object **&argv,
													Ink_Object *this_p, bool if_return_this,
													bool &if_delete_argv)
{
	Ink_ArgcType tmp_argc, j, argi;
	Ink_Object **tmp_argv;
	Ink_Object *ret_val = NULL;
	bool is_arg_completed = true;

	/* if some arguments have been applied already */
	if (pa_argv) {
		tmp_argc = pa_argc;
		tmp_argv = copyArgv(pa_argc, pa_argv);

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
				Ink_ArgcType remainc = argc - argi; /* remaining arguments */
				argc = remainc + tmp_argc;
				/* link the PA arguments and remaining arguments */
				argv = linkArgv(tmp_argc, tmp_argv,
								remainc, &argv[argi]);

				free(tmp_argv);
				tmp_argc = argc;
				tmp_argv = argv;
			}
			ret_val = cloneWithPA(engine, tmp_argc, tmp_argv,
								  this_p, if_return_this, true);
			goto RETURN;
		}

		Ink_ArgcType remainc = argc - argi; /* remaining arguments */
		argc = remainc + tmp_argc;
		/* link the PA arguments and remaining arguments */
		argv = linkArgv(tmp_argc, tmp_argv,
						remainc, &argv[argi]);
		free(tmp_argv);
		if_delete_argv = true;
	}

	for (argi = 0; argi < argc; argi++) {
		if (isUnknown(argv[argi])) { /* find unknown argument */
			ret_val = cloneWithPA(engine, argc, copyArgv(argc, argv),
								  this_p, if_return_this, true);
			goto RETURN;
		}
	}

RETURN:

	return ret_val;
}

Ink_Object *Ink_FunctionObject::call(Ink_InterpreteEngine *engine,
									 Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv,
									 Ink_Object *this_p, bool if_return_this)
{
	Ink_ExpressionList::size_type i;
	Ink_ArgcType argi;
	Ink_ParamList::size_type j;
	Ink_ContextObject *local;
	Ink_Object *ret_val = NULL, *pa_ret = NULL;
	Ink_Array *var_arg = NULL;
	IGC_CollectEngine *gc_engine_backup = engine->getCurrentGC();
	bool force_return = false;
	bool if_delete_argv = false;
	const char *debug_name_back = getDebugName();
	const char *base_debug_name_back = getSlot(engine, "base")->getDebugName();

	if ((pa_ret = checkUnkownArgument(argc, argv, this_p,
									  if_return_this, if_delete_argv))
		!= NULL) {
		if (if_delete_argv)
			free(argv);
		return pa_ret;
	}

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
	local->setSlot("let", local);

	/* set "this" pointer if exists */
	if (this_p)
		local->setSlot("this", this_p);

	// reset debug name
	getSlot(engine, "base")->setDebugName(base_debug_name_back);
	setDebugName(debug_name_back);

	context->addContext(local);

	/* set trace(unsed for mark&sweep GC) and set debug info */
	engine->addTrace(local)->setDebug(engine->current_file_name, engine->current_line_number, this);

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

		if (argi < argc) { /* still some parameter remaining */
			InkNote_Exceed_Argument(engine);
		}

		for (i = 0; i < exp_list.size(); i++) {
			gc_engine->checkGC();
			ret_val = exp_list[i]->eval(engine, context); // eval each expression

			/* interrupt signal received */
			if (engine->getSignal() != INTER_NONE) {
				/* interrupt event triggered */
				triggerInterruptEvent(engine, context, local, this);

				if (engine->getSignal() == INTER_NONE)
					continue;

				/* whether trap the signal */
				if (attr.hasTrap(engine->getSignal())) {
					ret_val = engine->trapSignal();
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
	if (ret_val) {
		engine->setGlobalReturnValue(ret_val);
		gc_engine->doMark(ret_val);
	}

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
	engine->setGlobalReturnValue(NULL);
	delete gc_engine;

	return ret_val ? ret_val : NULL_OBJ; // return the last expression
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
	new_obj->pa_argc = pa_argc;
	new_obj->pa_argv = copyArgv(pa_argc, pa_argv);
	new_obj->pa_info_this_p = pa_info_this_p;
	new_obj->pa_info_if_return_this = pa_info_if_return_this;

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
	new_obj->pa_argc = pa_argc;
	new_obj->pa_argv = copyDeepArgv(engine, pa_argc, pa_argv);
	new_obj->pa_info_this_p = pa_info_this_p ? pa_info_this_p->cloneDeep(engine) : NULL;
	new_obj->pa_info_if_return_this = pa_info_if_return_this;

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

void Ink_FunctionObject::doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker)
{
	Ink_ContextChain *global = closure_context ?
							   closure_context->getGlobal() :
							   NULL;
	Ink_ContextChain *j;
	Ink_ArgcType argi;

	for (j = global; j; j = j->inner) {
		marker(engine, j->context);
	}

	if (pa_argv) {
		for (argi = 0; argi < pa_argc; argi++) {
			marker(engine, pa_argv[argi]);
		}
	}

	if (pa_info_this_p) {
		marker(engine, pa_info_this_p);
	}

	return;
}

Ink_Object *Ink_Object::call(Ink_InterpreteEngine *engine,
							 Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv,
							 Ink_Object *this_p, bool if_return_this)
{
	InkError_Calling_Non_Function_Object(engine);
	return NULL_OBJ;
}

Ink_Object *Ink_Undefined::call(Ink_InterpreteEngine *engine,
								Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv,
								Ink_Object *this_p, bool if_return_this)
{
	InkError_Calling_Undefined_Object(engine);
	return NULL_OBJ;
}

Ink_FunctionObject::~Ink_FunctionObject()
{
	if (closure_context) Ink_ContextChain::disposeContextChain(closure_context);
	if (pa_argv) free(pa_argv);
	cleanHashTable();
}

}
