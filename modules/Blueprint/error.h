#ifndef _BP_ERROR_H_
#define _BP_ERROR_H_

#include "core/error.h"
#include "core/general.h"

extern Ink_ModuleID ink_native_blueprint_mod_id;

enum InkMod_Blueprint_ExceptionCode {
	INK_EXCODE_WARN_BLUEPRINT_IF_ARGUMENT_REQUIRE = INK_EXCODE_CUSTOM_START,
	INK_EXCODE_WARN_BLUEPRINT_IF_END_WITH_ELSE,
	INK_EXCODE_WARN_BLUEPRINT_IF_END_WITH_ELSE_IF,
	INK_EXCODE_WARN_BLUEPRINT_IF_END_WITH_ELSE_IF_HAS_CONDITION,
	INK_EXCODE_WARN_BLUEPRINT_IF_ELSE_IF_HAS_NO_CONDITION,
	INK_EXCODE_WARN_BLUEPRINT_WHILE_ARGUMENT_REQUIRE,
	INK_EXCODE_WARN_BLUEPRINT_WHILE_REQUIRE_REFERENCE_COND,
	INK_EXCODE_WARN_BLUEPRINT_WHILE_REQUIRE_BLOCK,
	INK_EXCODE_WARN_BLUEPRINT_FOR_ARGUMENT_REQUIRE,
	INK_EXCODE_WARN_BLUEPRINT_FOR_REQUIRE_REFERENCE_COND,
	INK_EXCODE_WARN_BLUEPRINT_FOR_REQUIRE_REFERENCE_INCR,
	INK_EXCODE_WARN_BLUEPRINT_FOR_REQUIRE_BLOCK,
	INK_EXCODE_WARN_BLUEPRINT_TRY_EXPECT_BLOCK_AFTER_CATCH,
	INK_EXCODE_WARN_BLUEPRINT_TRY_UNKNOWN_INSTR,
	INK_EXCODE_WARN_BLUEPRINT_TRY_NO_ARGUMENT_FOLLWING_CATCH,
	INK_EXCODE_WARN_BLUEPRINT_TRY_NO_ARGUMENT_FOLLWING_FINAL,
	INK_EXCODE_WARN_BLUEPRINT_TRY_EXPECT_BLOCK_AFTER_FINAL,
	INK_EXCODE_WARN_BLUEPRINT_TRY_UNKNOWN_INSTR_TYPE,
	INK_EXCODE_WARN_BLUEPRINT_ENV_NEED_STRING_ARGUMENT,
	INK_EXCODE_WARN_BLUEPRINT_ENV_NOT_A_PART_OF_ENV,
	INK_EXCODE_WARN_BLUEPRINT_ENV_FAILED_SET_ENV,
	INK_EXCODE_WARN_BLUEPRINT_MATH_SET_UNKNOWN_MODE,
	INK_EXCODE_WARN_BLUEPRINT_MATH_GET_UNKNOWN_MODE
};

/* if errors */
inline void
InkWarn_Blueprint_If_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_IF_ARGUMENT_REQUIRE,
						   "if function requires at least 1 argument");
	return;
}

inline void
InkWarn_Blueprint_If_End_With_Else(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_IF_END_WITH_ELSE,
						   "if function ended with else");
	return;
}

inline void
InkWarn_Blueprint_If_End_With_Else_If(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_IF_END_WITH_ELSE_IF,
						   "if function ended with else if(requires condition and block)");
	return;
}

inline void
InkWarn_Blueprint_If_End_With_Else_If_Has_Condition(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_IF_END_WITH_ELSE_IF_HAS_CONDITION,
						   "if function ended with else if(requires block)");
	return;
}

inline void
InkWarn_Blueprint_If_Else_If_Has_No_Condition(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_IF_ELSE_IF_HAS_NO_CONDITION,
						   "Condition for else if missing");
	return;
}

/* while errors */
inline void
InkWarn_Blueprint_While_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_WHILE_ARGUMENT_REQUIRE,
						   "while function requires at least 1 argument");
	return;
}

inline void
InkWarn_Blueprint_While_Require_Reference_Cond(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_WHILE_REQUIRE_REFERENCE_COND,
						   "while function requires reference as condition expression");
	return;
}

inline void
InkWarn_Blueprint_While_Require_Block(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_WHILE_REQUIRE_BLOCK,
						   "while function requires a block as the second argument");
	return;
}

/* for errors */
inline void
InkWarn_Blueprint_For_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_FOR_ARGUMENT_REQUIRE,
						   "for function requires at least 3 argument");
	return;
}

inline void
InkWarn_Blueprint_For_Require_Reference_Cond(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_FOR_REQUIRE_REFERENCE_COND,
						   "for function requires reference as condition expression");
	return;
}

inline void
InkWarn_Blueprint_For_Require_Reference_Incr(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_FOR_REQUIRE_REFERENCE_INCR,
						   "for function requires reference as increment expression");
	return;
}

inline void
InkWarn_Blueprint_For_Require_Block(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_FOR_REQUIRE_BLOCK,
						   "for function requires a block as the second argument");
	return;
}

/* try errors */
inline void
InkWarn_Blueprint_Try_Expect_Block_After_Catch(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_TRY_EXPECT_BLOCK_AFTER_CATCH,
						   "Expect block after catch in try function");
	return;
}

inline void
InkWarn_Blueprint_Try_Unknown_Instr(Ink_InterpreteEngine *engine, const char *instr)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_TRY_UNKNOWN_INSTR,
						   "Unknown instruction of try: \'$(instr)\'", instr);
	return;
}

inline void
InkWarn_Blueprint_Try_No_Argument_Follwing_Catch(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_TRY_NO_ARGUMENT_FOLLWING_CATCH,
						   "No argument following catch");
	return;
}

inline void
InkWarn_Blueprint_Try_No_Argument_Follwing_Final(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_TRY_NO_ARGUMENT_FOLLWING_FINAL,
						   "No argument following final");
	return;
}

inline void
InkWarn_Blueprint_Try_Expect_Block_After_Final(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_TRY_EXPECT_BLOCK_AFTER_FINAL,
						   "Expect block after final");
	return;
}

inline void
InkWarn_Blueprint_Try_Unknown_Instr_Type(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_TRY_UNKNOWN_INSTR_TYPE,
						   "Unknown instruction type of try: <$(type)>",
						   getTypeName(engine, type));
	return;
}

/* sys.env */
inline void
InkWarn_Blueprint_Env_Need_String_Argument(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_ENV_NEED_STRING_ARGUMENT,
						   "Setting environment variables needs string argument instead of that of type <$(type)>",
						   getTypeName(engine, type));
	return;
}

inline void
InkWarn_Blueprint_Env_Not_A_Part_Of_Env(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_ENV_NOT_A_PART_OF_ENV,
						   "Failed to set environment variable, please set sys.env object");
	return;
}

inline void
InkWarn_Blueprint_Env_Failed_Set_Env(Ink_InterpreteEngine *engine, const char *key)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_ENV_FAILED_SET_ENV,
						   "Failed to set environment variable $(key)", key);
	return;
}

/* math */
inline void
InkWarn_Blueprint_Math_Set_Unknown_Mode(Ink_InterpreteEngine *engine, const char *mode_str)
{
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_MATH_SET_UNKNOWN_MODE,
						   "Failed to set unknown mode $(mode)", mode_str);
	return;
}

inline void
InkWarn_Blueprint_Math_Get_Unknown_Mode(Ink_InterpreteEngine *engine, InkMod_Blueprint_Math_CalMode mode_e)
{
	stringstream strm;

	strm << "Failed to get unknown mode with code " << mode_e;
	InkErro_doPrintWarning(engine, ink_native_blueprint_mod_id,
						   INK_EXCODE_WARN_BLUEPRINT_MATH_GET_UNKNOWN_MODE,
						   strm.str().c_str());

	return;
}

#endif
