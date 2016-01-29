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

		if (argi > argc) { /* still some parameter remaining */
			InkWarn_Unfit_Argument(engine);
		}

		for (i = 0; i < exp_list.size(); i++) {
			gc_engine->checkGC();
			ret_val = exp_list[i]->eval(engine, context); // eval each expression

			/* interrupt signal received */
			if (engine->getSignal() != INTER_NONE) {
				/* interrupt event triggered */
				Ink_InterruptSignal signal_backup = engine->getSignal();
				Ink_Object *value_backup
							= local->ret_val /* set return value of context object for GC to mark */
							= engine->getInterruptValue();

				tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *));
				engine->setInterrupt(INTER_NONE, NULL);
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
					case INTER_RETRY:
						if ((tmp = getSlot(engine, "retry"))->type == INK_FUNCTION) {
							tmp_argv[0] = value_backup;
							callWithAttr(tmp, Ink_FunctionAttribution(INTER_NONE), engine, context, 1, tmp_argv);
						} break;
					default: ;
				}
				free(tmp_argv);
				/* restore signal if it hasn't been changed */
				if (engine->getSignal() == INTER_NONE) {
					engine->setInterrupt(signal_backup, value_backup);
				}

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

	if (partial_applied_argv) {
		for (argi = 0; argi < partial_applied_argc; argi++) {
			marker(engine, partial_applied_argv[argi]);
		}
	}
	return;
}

Ink_Object *Ink_Object::call(Ink_InterpreteEngine *engine,
							 Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv,
							 Ink_Object *this_p, bool if_return_this)
{
	InkErr_Calling_Non_Function_Object(engine);
	return NULL_OBJ;
}

Ink_Object *Ink_Undefined::call(Ink_InterpreteEngine *engine,
								Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv,
								Ink_Object *this_p, bool if_return_this)
{
	InkErr_Calling_Undefined_Object(engine);
	return NULL_OBJ;
}

Ink_FunctionObject::~Ink_FunctionObject()
{
	if (closure_context) Ink_ContextChain::disposeContextChain(closure_context);
	if (partial_applied_argv) free(partial_applied_argv);
	cleanHashTable();
}

}