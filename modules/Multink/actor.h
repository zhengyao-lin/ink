#ifndef _NATIVE_ACTOR_H_
#define _NATIVE_ACTOR_H_

#include "core/actor.h"
#include "core/error.h"
#include "core/object.h"

class Ink_ActorFunction_sub_Argument {
public:
	Ink_InterpreteEngine *engine;
	Ink_ExpressionList exp_list;
	Ink_ParamList param_list;
	unsigned int argc;
	Ink_Object **argv;

	Ink_ActorFunction_sub_Argument(Ink_InterpreteEngine *engine, Ink_ExpressionList exp_list,
								   Ink_ParamList param_list, unsigned int argc, Ink_Object **argv)
	: engine(engine), exp_list(exp_list), param_list(param_list), argc(argc), argv(argv)
	{ }
};

class Ink_ActorFunction: public Ink_FunctionObject {
public:

	Ink_ActorFunction(Ink_InterpreteEngine *engine, Ink_ParamList param_list, Ink_ExpressionList exp_list)
	: Ink_FunctionObject(engine, param_list, exp_list, NULL)
	{ }

	virtual Ink_Object *call(Ink_InterpreteEngine *engine,
							 Ink_ContextChain *context, unsigned int argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true);
};

inline void
InkWarn_Multink_Actor_Not_Found(Ink_InterpreteEngine *engine, const char *name)
{
	InkWarn_doPrintWarning(engine, "Cannot find actor \'$(name)\'", name);
	return;
}

inline void
InkWarn_Multink_Message_is_not_a_String(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Message is not a string");
	return;
}

inline void
InkWarn_Multink_Not_String_Argument(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Argument is not a string(may cause crash)");
	return;
}

inline void
InkWarn_Multink_Instruction_Argument_Require(Ink_InterpreteEngine *engine, const char *instr)
{
	InkWarn_doPrintWarning(engine, "\'$(instr)\' instruction is given, but no argument given", instr);
	return;
}

inline void
InkWarn_Multink_Wrong_Argument_Type(Ink_InterpreteEngine *engine, const char *instr)
{
	InkWarn_doPrintWarning(engine, "Wrong argument type for instruction \'$(instr)\'", instr);
	return;
}

inline void
InkWarn_Multink_Unknown_Instruction(Ink_InterpreteEngine *engine, const char *instr)
{
	InkWarn_doPrintWarning(engine, "Unknown instruction \'$(instr)\'", instr);
	return;
}

inline void
InkWarn_Multink_Expect_Instruction(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Expecting instruction");
	return;
}

#endif