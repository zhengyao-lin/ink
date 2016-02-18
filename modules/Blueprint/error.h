#ifndef _BP_ERROR_H_
#define _BP_ERROR_H_

#include "core/error.h"

/* if errors */
inline void
InkWarn_Blueprint_If_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "if function requires at least 1 argument");
	return;
}

inline void
InkWarn_Blueprint_If_End_With_Else(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "if function ended with else");
	return;
}

inline void
InkWarn_Blueprint_If_End_With_Else_If(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "if function ended with else if(requires condition and block)");
	return;
}

inline void
InkWarn_Blueprint_If_End_With_Else_If_Has_Condition(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "if function ended with else if(requires block)");
	return;
}

inline void
InkWarn_Blueprint_If_Else_If_Has_No_Condition(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "Condition for else if missing");
	return;
}

/* while errors */
inline void
InkWarn_Blueprint_While_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "while function requires at least 1 argument");
	return;
}

inline void
InkWarn_Blueprint_While_Require_Reference_Cond(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "while function requires reference as condition expression");
	return;
}

inline void
InkWarn_Blueprint_While_Require_Block(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "while function requires a block as the second argument");
	return;
}

/* for errors */
inline void
InkWarn_Blueprint_For_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "for function requires at least 3 argument");
	return;
}

inline void
InkWarn_Blueprint_For_Require_Reference_Cond(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "for function requires reference as condition expression");
	return;
}

inline void
InkWarn_Blueprint_For_Require_Reference_Incr(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "for function requires reference as increment expression");
	return;
}

inline void
InkWarn_Blueprint_For_Require_Block(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "for function requires a block as the second argument");
	return;
}

/* try errors */
inline void
InkWarn_Blueprint_Try_Expect_Block_After_Catch(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "Expect block after catch in try function");
	return;
}

inline void
InkWarn_Blueprint_Try_Unknown_Instr(Ink_InterpreteEngine *engine, const char *instr)
{
	InkErro_doPrintWarning(engine, "Unknown instruction of try: \'$(instr)\'", instr);
	return;
}

inline void
InkWarn_Blueprint_Try_No_Argument_Follwing_Catch(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "No argument following catch");
	return;
}

inline void
InkWarn_Blueprint_Try_No_Argument_Follwing_Final(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "No argument following final");
	return;
}

inline void
InkWarn_Blueprint_Try_Expect_Block_After_Final(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "Expect block after final");
	return;
}

inline void
InkWarn_Blueprint_Try_Unknown_Instr_Type(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, "Unknown instruction type of try: <$(type)>",
						   getTypeName(engine, type));
	return;
}

/* sys.env */
inline void
InkWarn_Blueprint_Env_Need_String_Argument(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, "Setting environment variables needs string argument instead of that of type <$(type)>",
						   getTypeName(engine, type));
	return;
}

inline void
InkWarn_Blueprint_Env_Not_A_Part_Of_Env(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "Failed to set environment variable, please set sys.env object");
	return;
}

inline void
InkWarn_Blueprint_Env_Failed_Set_Env(Ink_InterpreteEngine *engine, const char *key)
{
	InkErro_doPrintWarning(engine, "Failed to set environment variable $(key)", key);
	return;
}

#endif
