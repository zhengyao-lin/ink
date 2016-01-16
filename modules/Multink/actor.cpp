#include "actor.h"
#include "core/object.h"
#include "core/general.h"
#include "core/expression.h"
#include "core/protocol.h"
#include "core/context.h"
#include "core/native/native.h"
#include "core/interface/engine.h"
#include "core/thread/thread.h"

using namespace std;

Ink_Object *InkNative_Actor_Send_Sub(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	Ink_Object *msg = base->getSlot(engine, "msg");
	if (msg->type != INK_STRING) {
		InkWarn_Message_is_not_a_String(engine);
		return NULL_OBJ;
	}

	Ink_InterpreteEngine *dest = InkActor_getActor(as<Ink_String>(argv[0])->getValue());
	if (!dest) {
		InkWarn_Actor_Not_Found(engine, as<Ink_String>(argv[0])->getValue().c_str());
		return NULL_OBJ;
	}

	dest->sendInMessage(as<Ink_String>(msg)->getValue());

	return TRUE_OBJ;
}

Ink_Object *InkNative_Actor_Send(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}
	
	Ink_Object *sender = new Ink_Object(engine);
	sender->setSlot("->", new Ink_FunctionObject(engine, InkNative_Actor_Send_Sub));
	sender->setSlot("msg", argv[0]);

	return sender;
}

Ink_Object *InkNative_Actor_Receive(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *msg = engine->receiveMessage();
	if (!msg) {
		return NULL_OBJ;
	}

	return msg;
}

Ink_Object *InkNative_Actor_JoinAll(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	InkActor_joinAllActor(engine);
	return NULL_OBJ;
}

Ink_Object *InkNative_Actor_JoinAllBut(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	Ink_InterpreteEngine *dest = InkActor_getActor(as<Ink_String>(argv[0])->getValue());
	if (!dest) {
		InkWarn_Actor_Not_Found(engine, as<Ink_String>(argv[0])->getValue().c_str());
		return NULL_OBJ;
	}

	InkActor_joinAllActor(engine, dest);
	
	return NULL_OBJ;
}

Ink_Object *InkNative_Actor_ActorCount(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	return new Ink_Numeric(engine, InkActor_getActorCount());
}

void *Ink_ActorFunction_sub(void *arg)
{
	Ink_ActorFunction_sub_Argument *tmp = (Ink_ActorFunction_sub_Argument *)arg;
	tmp->engine->top_level = tmp->exp_list;

	tmp->engine->execute();

	tmp->engine->top_level = Ink_ExpressionList();
	InkActor_setDeadActor(tmp->engine);
	delete tmp->engine;
	delete tmp;

	return NULL;
}

Ink_Object *Ink_ActorFunction::call(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv,
									Ink_Object *this_p, bool if_return_this)
{
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	Ink_InterpreteEngine *new_engine;
	pthread_t new_thread;
	Ink_InterpreteEngine *check;
	Ink_ActorFunction_sub_Argument *tmp_arg;

	if ((check = InkActor_getActor(as<Ink_String>(argv[0])->getValue())) != NULL) {
		InkWarn_Actor_Conflict(engine, as<Ink_String>(argv[0])->getValue().c_str());
		return NULL_OBJ;
	}

	new_engine = new Ink_InterpreteEngine();
	int ret_val = pthread_create(&new_thread, NULL, Ink_ActorFunction_sub,
							     tmp_arg = new Ink_ActorFunction_sub_Argument(new_engine, exp_list));

	if (ret_val) {
		InkWarn_Failed_Create_Process(engine, ret_val);
		delete tmp_arg;
		delete new_engine;
		return NULL_OBJ;
	}

	pthread_detach(new_thread);
	string *name = new string(as<Ink_String>(argv[0])->getValue().c_str());
	InkActor_addActor(*name, new_engine, new_thread, name);

	return TRUE_OBJ;
}

Ink_FunctionObject *InkMod_Actor_ActorProtocol(Ink_InterpreteEngine *engine, Ink_ParamList param, Ink_ExpressionList exp_list, Ink_ContextChain *closure_context)
{
	Ink_ContextChain::disposeContextChain(closure_context);
	return new Ink_ActorFunction(engine, exp_list);
}

void InkMod_Actor_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot("send", new Ink_FunctionObject(engine, InkNative_Actor_Send));
	bondee->setSlot("receive", new Ink_FunctionObject(engine, InkNative_Actor_Receive));
	bondee->setSlot("join_all", new Ink_FunctionObject(engine, InkNative_Actor_JoinAll));
	bondee->setSlot("join_all_but", new Ink_FunctionObject(engine, InkNative_Actor_JoinAllBut));
	bondee->setSlot("actor_count", new Ink_FunctionObject(engine, InkNative_Actor_ActorCount));

	return;
}

Ink_Object *InkMod_Actor_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *global_context = context->getGlobal()->context;
	InkMod_Actor_bondTo(engine, global_context);
	return NULL_OBJ;
}

extern "C" {
	void InkMod_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
	{
		Ink_Object *actor_obj = addPackage(engine, context, "multink", new Ink_FunctionObject(engine, InkMod_Actor_Loader));
		InkMod_Actor_bondTo(engine, actor_obj);
		engine->addProtocol("actor", InkMod_Actor_ActorProtocol);
		return;
	}
}