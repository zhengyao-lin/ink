#include <signal.h>
#include "actor.h"
#include "core/object.h"
#include "core/general.h"
#include "core/expression.h"
#include "core/protocol.h"
#include "core/context.h"
#include "core/time.h"
#include "core/coroutine/coroutine.h"
#include "core/native/native.h"
#include "core/interface/engine.h"
#include "core/thread/thread.h"

using namespace ink;
using namespace std;

Ink_ModuleID ink_native_multink_mod_id;

Ink_Object *InkNative_Actor_Send_Sub(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{

	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	Ink_Object *msg = base->getSlot(engine, "msg");
	if (msg->type != INK_STRING) {
		InkWarn_Multink_Message_is_not_a_String(engine);
		return NULL_OBJ;
	}

	string tmp = as<Ink_String>(argv[0])->getValue();

	InkActor_lockActorLock();
	Ink_InterpreteEngine *dest = InkActor_getActor_nolock(tmp);
	if (!dest) {
		InkActor_unlockActorLock();
		InkWarn_Multink_Actor_Not_Found(engine, tmp.c_str());
		return NULL_OBJ;
	}

	dest->sendInMessage_nolock(engine, as<Ink_String>(msg)->getValue());
	InkActor_unlockActorLock();

	return TRUE_OBJ;
}

Ink_Object *InkNative_Actor_Send(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}
	
	Ink_Object *sender = new Ink_Object(engine);
	sender->setSlot_c("->", new Ink_FunctionObject(engine, InkNative_Actor_Send_Sub));
	sender->setSlot_c("msg", argv[0]);

	return sender;
}

Ink_Object *InkNative_Actor_Receive(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	bool if_wait_forever = false;
	Ink_MilliSec time_begin = -1, max_time = -1, delay = -1;
	Ink_ArgcType i;
	string tmp_instr;
	Ink_ArrayValue arr_val;
	Ink_Object *tmp_obj, *msg;

	for (i = 0; i < argc; i++) {
		if (argv[i]->type == INK_STRING) {
			tmp_instr = as<Ink_String>(argv[i])->getValue();
			if (tmp_instr == "every") {
				if (argv[i + 1]->type == INK_ARRAY
					&& (arr_val = as<Ink_Array>(argv[i + 1])->value).size()) {
					if ((tmp_obj = arr_val[0]->getValue())->type == INK_NUMERIC) {
						delay = getInt(as<Ink_Numeric>(tmp_obj)->getValue());
						i++;
					} else {
						InkWarn_Multink_Wrong_Argument_Type(engine, "every");
						return NULL_OBJ;
					}
				} else {
					InkWarn_Multink_Instruction_Argument_Require(engine, "every");
					return NULL_OBJ;
				}
			} else if (tmp_instr == "for") {
				if (argv[i + 1]->type == INK_ARRAY
					&& (arr_val = as<Ink_Array>(argv[i + 1])->value).size()) {
					if ((tmp_obj = arr_val[0]->getValue())->type == INK_NUMERIC) {
						max_time = getInt(as<Ink_Numeric>(tmp_obj)->getValue());
						i++;
					} else {
						InkWarn_Multink_Wrong_Argument_Type(engine, "for");
						return NULL_OBJ;
					}
				} else {
					InkWarn_Multink_Instruction_Argument_Require(engine, "for");
					return NULL_OBJ;
				}
			} else if (tmp_instr == "forever") {
				if_wait_forever = true;
				i++;
			} else {
				InkWarn_Multink_Unknown_Instruction(engine, tmp_instr.c_str());
				return NULL_OBJ;
			}
		} else {
			InkWarn_Multink_Expect_Instruction(engine);
			return NULL_OBJ;
		}
	}

	if (delay < 0 && max_time < 0 && !if_wait_forever) {
		msg = engine->receiveMessage();
		if (!msg) {
			return NULL_OBJ;
		}
	} else {
		time_begin = Ink_getCurrentMS();
		while (!(msg = engine->receiveMessage())) {
			if (!if_wait_forever
				&& max_time >= 0
				&& Ink_getCurrentMS() - time_begin >= max_time)
				break;
			if (delay >= 0) Ink_msleep(delay);
		}
	}

	return msg;
}

Ink_Object *InkNative_Actor_JoinAll(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	InkActor_joinAllActor(engine);
	return NULL_OBJ;
}

Ink_Object *InkNative_Actor_JoinAllBut(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	string tmp = as<Ink_String>(argv[0])->getValue();
	Ink_InterpreteEngine *dest = InkActor_getActor(as<Ink_String>(argv[0])->getValue());
	if (!dest) {
		InkWarn_Multink_Actor_Not_Found(engine, tmp.c_str());
		return NULL_OBJ;
	}

	InkActor_joinAllActor(engine, dest);
	
	return NULL_OBJ;
}

Ink_Object *InkNative_Actor_ActorCount(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	return new Ink_Numeric(engine, InkActor_getActorCount());
}

Ink_Object *InkNative_Actor_ActorSelf(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	string *tmp = InkActor_getActorName(engine);
	if (!tmp) {
		InkWarn_Multink_Require_Registered_Actor(engine);
		return NULL_OBJ;
	}
	return new Ink_String(engine, tmp);
}

Ink_Object *InkNative_Actor_ActorExist(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	Ink_InterpreteEngine *dest = InkActor_getActor(as<Ink_String>(argv[0])->getValue());
	return new Ink_Numeric(engine, dest != NULL);
}

Ink_Object *InkNative_Actor_ActorWatch(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	string tmp = as<Ink_String>(argv[0])->getValue();

	InkActor_lockActorLock();
	Ink_InterpreteEngine *dest = InkActor_getActor_nolock(tmp);

	if (!dest) {
		InkActor_unlockActorLock();
		InkWarn_Multink_Actor_Not_Found(engine, tmp.c_str());
		return NULL_OBJ;
	}

	string *self = InkActor_getActorName_nolock(engine);

	if (!self) {
		InkActor_unlockActorLock();
		InkWarn_Multink_Require_Registered_Actor(engine);
		return NULL_OBJ;
	}

	dest->addWatcher(*self);
	InkActor_unlockActorLock();

	delete self;

	return TRUE_OBJ;
}

void Ink_ActorFunction_signal_handler(int errno)
{
	return;
}

void Ink_ActorFunction_cleanup(Ink_InterpreteEngine *engine, void *arg)
{
	Ink_ActorFunction_sub_Argument *tmp = (Ink_ActorFunction_sub_Argument *)arg;
	tmp->engine->top_level = Ink_ExpressionList();
	InkActor_setDeadActor(tmp->engine);
	if (tmp->argv)
		free(tmp->argv);
	delete tmp;
	return;
}

void *Ink_ActorFunction_sub(void *arg)
{
	InkActor_lockThreadCreateLock();
	InkActor_unlockThreadCreateLock();

	// pthread_cleanup_push(Ink_ActorFunction_cleanup, arg);
	// signal(SIGABRT, Ink_ActorFunction_signal_handler);
	// pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	// pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

	Ink_ActorFunction_sub_Argument *tmp = (Ink_ActorFunction_sub_Argument *)arg;
	Ink_InterpreteEngine *engine = tmp->engine;
	tmp->engine->top_level = tmp->exp_list;
	Ink_ArgcType argi;
	Ink_ParamList::size_type j;
	Ink_Array *var_arg = NULL;
	Ink_ContextObject *local = tmp->engine->global_context->getLocal();

	Ink_initCoroutine();

	engine->addDestructor(Ink_EngineDestructor(Ink_ActorFunction_cleanup, arg));

	for (j = 0, argi = 0; j < tmp->param_list.size(); j++, argi++) {
		if (tmp->param_list[j].is_variant) { /* find variant argument -- break the loop */
			break;
		}
		local->setSlot(tmp->param_list[j].name->c_str(),
					   argi < tmp->argc ? tmp->argv[argi]
					   			   : UNDEFINED); // initiate local argument
	}

	if (j < tmp->param_list.size()
		&& tmp->param_list[j].is_variant) {
		/* breaked from finding variant arguments */

		/* create variant arguments */
		var_arg = new Ink_Array(tmp->engine);
		for (; argi < tmp->argc; argi++) {
			/* push arguments in to VA array */
			var_arg->value.push_back(new Ink_HashTable("", tmp->argv[argi]));
		}

		/* set VA array */
		local->setSlot(tmp->param_list[j].name->c_str(), var_arg);
	}

	if (argi < tmp->argc) { /* still some parameter remaining */
		InkNote_Exceed_Argument(tmp->engine);
	}

	tmp->engine->execute();

	delete engine;

	return NULL;
}

Ink_Object *Ink_ActorFunction::call(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
									Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv,
									Ink_Object *this_p, bool if_return_this)
{
	Ink_InterpreteEngine *new_engine;
	pthread_t new_thread;
	Ink_ActorFunction_sub_Argument *tmp_arg;
	Ink_ArgcType tmp_argc, i;
	Ink_Object **tmp_argv, *pa_ret;
	bool if_delete_argv = false;

	if ((pa_ret = checkUnkownArgument(base, argc, argv, this_p,
									  if_return_this, if_delete_argv))
		!= NULL) {
		if (if_delete_argv)
			free(argv);
		return pa_ret;
	}

	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	string tmp_str = as<Ink_String>(argv[0])->getValue();

	if (InkActor_getActor(tmp_str)) {
		InkWarn_Actor_Conflict(engine, tmp_str.c_str());
		return NULL_OBJ;
	}

	new_engine = new Ink_InterpreteEngine();

	tmp_argc = argc - 1;
	tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *) * (argc - 1));
	for (i = 1; i < argc; i++) {
		new_engine->initDeepClone();
		tmp_argv[i - 1] = argv[i]->cloneDeep(new_engine);
	}
	tmp_arg = new Ink_ActorFunction_sub_Argument(new_engine, exp_list,
												 param, tmp_argc, tmp_argv);

	InkActor_lockThreadCreateLock();
	int ret_val = pthread_create(&new_thread, NULL, Ink_ActorFunction_sub, tmp_arg);

	if (ret_val) {
		InkWarn_Failed_Create_Process(engine, ret_val);
		InkActor_unlockThreadCreateLock();
		delete tmp_arg;
		delete new_engine;
		return NULL_OBJ;
	}

	pthread_detach(new_thread);
	string *name = new string(tmp_str.c_str());
	InkActor_addActor(*name, new_engine, new_thread, name);
	InkActor_unlockThreadCreateLock();

	if (if_delete_argv) {
		free(argv);
	}

	return TRUE_OBJ;
}

Ink_Object *Ink_ActorFunction::clone(Ink_InterpreteEngine *engine)
{
	Ink_FunctionObject *new_obj = new Ink_ActorFunction(engine);
	
	new_obj->is_native = is_native;
	new_obj->is_inline = is_inline;
	new_obj->is_ref = is_ref;
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

Ink_Object *Ink_ActorFunction::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_FunctionObject *new_obj = new Ink_ActorFunction(engine);
	
	new_obj->is_native = is_native;
	new_obj->is_inline = is_inline;
	new_obj->is_ref = is_ref;
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

Ink_FunctionObject *InkMod_Actor_ActorProtocol(Ink_InterpreteEngine *engine, Ink_ParamList param, Ink_ExpressionList exp_list, Ink_ContextChain *closure_context)
{
	Ink_ContextChain::disposeContextChain(closure_context);
	return new Ink_ActorFunction(engine, param, exp_list);
}

void InkMod_Actor_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot_c("send", new Ink_FunctionObject(engine, InkNative_Actor_Send));
	bondee->setSlot_c("receive", new Ink_FunctionObject(engine, InkNative_Actor_Receive));
	bondee->setSlot_c("join_all", new Ink_FunctionObject(engine, InkNative_Actor_JoinAll));
	bondee->setSlot_c("join_all_but", new Ink_FunctionObject(engine, InkNative_Actor_JoinAllBut));
	bondee->setSlot_c("actor_count", new Ink_FunctionObject(engine, InkNative_Actor_ActorCount));
	bondee->setSlot_c("actor_self", new Ink_FunctionObject(engine, InkNative_Actor_ActorSelf));
	bondee->setSlot_c("actor_exist", new Ink_FunctionObject(engine, InkNative_Actor_ActorExist));
	bondee->setSlot_c("actor_watch", new Ink_FunctionObject(engine, InkNative_Actor_ActorWatch));

	return;
}

Ink_Object *InkMod_Actor_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];

	InkMod_Actor_bondTo(engine, apply_to);
	
	return NULL_OBJ;
}

extern "C" {
	void InkMod_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
	{
		addPackage(engine, context, "multink", new Ink_FunctionObject(engine, InkMod_Actor_Loader));
		engine->addProtocol("actor", InkMod_Actor_ActorProtocol);

		return;
	}

	int InkMod_Init(Ink_ModuleID id)
	{
		ink_native_multink_mod_id = id;
		return 0;
	}
}
