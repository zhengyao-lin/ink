#ifndef _NATIVE_ACTOR_H_
#define _NATIVE_ACTOR_H_

#include "core/error.h"
#include "core/object.h"
#include "core/thread/actor.h"

using namespace ink;

class Ink_ActorFunction_sub_Argument {
public:
	Ink_InterpreteEngine *engine;
	Ink_ExpressionList exp_list;
	Ink_ParamList param_list;
	Ink_ArgcType argc;
	Ink_Object **argv;

	Ink_ActorFunction_sub_Argument(Ink_InterpreteEngine *engine, Ink_ExpressionList exp_list,
								   Ink_ParamList param_list, Ink_ArgcType argc, Ink_Object **argv)
	: engine(engine), exp_list(exp_list), param_list(param_list),
	  argc(argc), argv(argv)
	{ }
};

class Ink_ActorFunction: public Ink_FunctionObject {
public:

	Ink_ActorFunction(Ink_InterpreteEngine *engine)
	: Ink_FunctionObject(engine, Ink_ParamList(), Ink_ExpressionList(), NULL)
	{ }

	Ink_ActorFunction(Ink_InterpreteEngine *engine, Ink_ParamList param_list, Ink_ExpressionList exp_list)
	: Ink_FunctionObject(engine, param_list, exp_list, NULL)
	{ }

	virtual Ink_Object *call(Ink_InterpreteEngine *engine, Ink_ContextChain *context,
							 Ink_Object *base = NULL, Ink_ArgcType argc = 0, Ink_Object **argv = NULL,
							 Ink_Object *this_p = NULL, bool if_return_this = true);
	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
};

extern Ink_ModuleID ink_native_multink_mod_id;

enum InkMod_Multink_ExceptionCode {
	INK_EXCODE_WARN_MULTINK_ACTOR_NOT_FOUND = INK_EXCODE_CUSTOM_START,
	INK_EXCODE_WARN_MULTINK_MESSAGE_IS_NOT_A_STRING,
	INK_EXCODE_WARN_MULTINK_NOT_STRING_ARGUMENT,
	INK_EXCODE_WARN_MULTINK_INSTRUCTION_ARGUMENT_REQUIRE,
	INK_EXCODE_WARN_MULTINK_WRONG_ARGUMENT_TYPE,
	INK_EXCODE_WARN_MULTINK_UNKNOWN_INSTRUCTION,
	INK_EXCODE_WARN_MULTINK_EXPECT_INSTRUCTION,
	INK_EXCODE_WARN_MULTINK_REQUIRE_REGISTERED_ACTOR
};

inline void
InkWarn_Multink_Actor_Not_Found(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, ink_native_multink_mod_id,
						   INK_EXCODE_WARN_MULTINK_ACTOR_NOT_FOUND,
						   "Cannot find actor \'$(name)\'", name);
	return;
}

inline void
InkWarn_Multink_Message_is_not_a_String(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_multink_mod_id,
						   INK_EXCODE_WARN_MULTINK_MESSAGE_IS_NOT_A_STRING,
						   "Message is not a string");
	return;
}

inline void
InkWarn_Multink_Not_String_Argument(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_multink_mod_id,
						   INK_EXCODE_WARN_MULTINK_NOT_STRING_ARGUMENT,
						   "Argument is not a string(may cause crash)");
	return;
}

inline void
InkWarn_Multink_Instruction_Argument_Require(Ink_InterpreteEngine *engine, const char *instr)
{
	InkErro_doPrintWarning(engine, ink_native_multink_mod_id,
						   INK_EXCODE_WARN_MULTINK_INSTRUCTION_ARGUMENT_REQUIRE,
						   "\'$(instr)\' instruction is given, but no argument given", instr);
	return;
}

inline void
InkWarn_Multink_Wrong_Argument_Type(Ink_InterpreteEngine *engine, const char *instr)
{
	InkErro_doPrintWarning(engine, ink_native_multink_mod_id,
						   INK_EXCODE_WARN_MULTINK_WRONG_ARGUMENT_TYPE,
						   "Wrong argument type for instruction \'$(instr)\'", instr);
	return;
}

inline void
InkWarn_Multink_Unknown_Instruction(Ink_InterpreteEngine *engine, const char *instr)
{
	InkErro_doPrintWarning(engine, ink_native_multink_mod_id,
						   INK_EXCODE_WARN_MULTINK_UNKNOWN_INSTRUCTION,
						   "Unknown instruction \'$(instr)\'", instr);
	return;
}

inline void
InkWarn_Multink_Expect_Instruction(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_multink_mod_id,
						   INK_EXCODE_WARN_MULTINK_EXPECT_INSTRUCTION,
						   "Expecting instruction");
	return;
}

inline void
InkWarn_Multink_Require_Registered_Actor(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_multink_mod_id,
						   INK_EXCODE_WARN_MULTINK_REQUIRE_REGISTERED_ACTOR,
						   "Only actors can watch or get self name");
	return;
}

#endif
