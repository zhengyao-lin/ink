#ifndef _ERROR_H_
#define _ERROR_H_

#include <sstream>
#include <iostream>
#include <string>
#include "type.h"
#include "debug.h"
#include "general.h"
#include "msg/emcore.h"

namespace ink {

class Ink_ContextChain;
class Ink_InterpreteEngine;

void InkErro_doPrintError(const char *msg, ...);
void InkErro_doPrintError(Ink_InterpreteEngine *engine, const char *msg, ...);
void InkErro_doPrintError(Ink_InterpreteEngine *engine, Ink_ExceptionCode ex_code, const char *msg, ...);
void InkErro_doPrintError(Ink_InterpreteEngine *engine, Ink_ModuleID mod_id, Ink_ExceptionCode ex_code, const char *msg, ...);

void InkErro_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg, ...);
void InkErro_doPrintWarning(Ink_InterpreteEngine *engine, Ink_ExceptionCode ex_code, const char *msg, ...);
void InkErro_doPrintWarning(Ink_InterpreteEngine *engine, Ink_ModuleID mod_id, Ink_ExceptionCode ex_code, const char *msg, ...);

void InkErro_doPrintNote(Ink_InterpreteEngine *engine, const char *msg, ...);

const char *getTypeName(Ink_InterpreteEngine *engine, Ink_TypeTag type);

inline void
InkError_Segment_Fault()
{
	InkErro_doPrintError("SEGV signal received");
	return;
}

inline void
InkError_Calling_Non_Function_Object(Ink_InterpreteEngine *engine, Ink_TypeTag type, const char *slot)
{
	InkErro_doPrintError(engine, INK_EXCODE_ERROR_CALLING_NON_FUNCTION_OBJECT,
						 "Calling non-function object of type <$(type)> in $(slot)",
						 getTypeName(engine, type),
						 (slot ? std::string("slot '") + slot + "'" : "anonymous slot").c_str());
	return;
}

inline void
InkError_Calling_Undefined_Object(Ink_InterpreteEngine *engine, const char *slot)
{
	InkErro_doPrintError(engine, INK_EXCODE_ERROR_CALLING_UNDEFINED_OBJECT,
						 "Calling undefined object in $(slot)",
						 (slot ? std::string("slot '") + slot + "'" : "anonymous slot").c_str());
	return;
}

inline void
InkError_Yield_Without_Coroutine(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintError(engine, INK_EXCODE_ERROR_YIELD_WITHOUT_COROUTINE,
						 "Function yield, but no coroutine is created");
	return;
}

inline void
InkError_Failed_Open_File(Ink_InterpreteEngine *engine, const char *path)
{
	InkErro_doPrintError(engine, INK_EXCODE_ERROR_FAILED_OPEN_FILE,
						 "Failed to open file $(path)", path);
	return;
}

inline void
InkWarn_Assigning_Unassignable_Expression(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_ASSIGNING_UNASSIGNABLE_EXPRESSION,
						   "Assigning unassignable expression");
	return;
}

inline void
InkWarn_Hash_not_found(Ink_InterpreteEngine *engine, const char *index)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_HASH_NOT_FOUND,
						   "Creating undefined slot \'$(index)\'", index);
	return;
}

inline void
InkWarn_Get_Slot_Of_Undefined(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_GET_SLOT_OF_UNDEFINED,
						   "Getting slot \'$(name)\' of undefined value", name);
	return;
}

inline void
InkWarn_Insert_Non_Function_Object(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_INSERT_NON_FUNCTION_OBJECT,
						   "Inserting non-function or non-expression-list object");
	return;
}

inline void
InkWarn_Array_Index_Exceed(Ink_InterpreteEngine *engine,
						   Ink_ArrayValue::size_type index,
						   Ink_ArrayValue::size_type size)
{
	std::stringstream strm;
	strm << "Index " << index << " exceed size of the array(" << size << ")";
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_ARRAY_INDEX_EXCEED,
						   strm.str().c_str());
	return;
}

inline void
InkWarn_Bonding_Failed(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_BONDING_FAILED,
						   "Failed to bond");
	return;
}

inline void
InkWarn_Self_Bonding(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_SELF_BONDING,
						   "Self bonding detected(may cause dead loop)");
	return;
}

inline void
InkWarn_Get_Non_Array_Index(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_GET_NON_ARRAY_INDEX,
						   "Getting element of non-array");
	return;
}

inline void
InkWarn_Function_Range_Call_Argument_Error(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FUNCTION_RANGE_CALL_ARGUMENT_ERROR,
						   "Function range call requires at least 1 argument");
	return;
}

inline void
InkWarn_Function_Non_Range_Call(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FUNCTION_NON_RANGE_CALL,
						   "Function range call requires argument of range type");
	return;
}

inline void
InkWarn_Incorrect_Range_Type(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_INCORRECT_RANGE_TYPE,
						   "Incorrect range type");
	return;
}

inline void
InkWarn_Get_Non_Function_Exp(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_GET_NON_FUNCTION_EXP,
						   "Getting expressions of non-function object");
	return;
}

inline void
InkWarn_Rebuild_Non_Array(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_REBUILD_NON_ARRAY,
						   "Rebuilding non-array object");
	return;
}

inline void
InkWarn_Invalid_Element_For_Rebuild(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_INVALID_ELEMENT_FOR_REBUILD,
						   "Invalid element for rebuilding");
	return;
}

inline void
InkWarn_Remove_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_REMOVE_ARGUMENT_REQUIRE,
						   "Remove method need at least one numeric argument");
	return;
}

inline void
InkWarn_Too_Huge_Index(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_TOO_HUGE_INDEX,
						   "Index greater than size");
	return;
}

inline void
InkWarn_Each_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_EACH_ARGUMENT_REQUIRE,
						   "Each function requires at least one function argument");
	return;
}

inline void
InkWarn_Failed_Finding_Method(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FAILED_FINDING_METHOD,
						   "Failed to find method $(name)", name);
	return;
}

inline void
InkWarn_Wrong_Type(Ink_InterpreteEngine *engine, Ink_TypeTag expect, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_WRONG_TYPE,
						   "Expecting type <$(expect)>, <$(type)> given",
						   getTypeName(engine, expect), getTypeName(engine, type));
	return;
}

inline void
InkWarn_Wrong_Base_Type(Ink_InterpreteEngine *engine, Ink_TypeTag expect, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_WRONG_BASE_TYPE,
						   "Expecting base of type <$(expect)>, that of type <$(type)> is given",
						   getTypeName(engine, expect), getTypeName(engine, type));
	return;
}

inline void
InkWarn_Too_Less_Argument(Ink_InterpreteEngine *engine, Ink_ArgcType min, Ink_ArgcType argc)
{
	std::stringstream strm;
	strm << "Too less argument. need at least " << min
		 << ", " << argc << " given";

	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_TOO_LESS_ARGUMENT,
						   strm.str().c_str());
	return;
}

inline void
InkWarn_Wrong_Argument_Type(Ink_InterpreteEngine *engine, Ink_TypeTag expect, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_WRONG_ARGUMENT_TYPE,
						   "Expecting object of type <$(expect)>, that of <$(type)> given",
						   getTypeName(engine, expect), getTypeName(engine, type));
	return;
}

inline void
InkWarn_Not_Package(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_NOT_PACKAGE,
						   "Object is not a package(with load method)");
	return;
}

inline void
InkWarn_Setting_Unassignable_Getter(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_SETTING_UNASSIGNABLE_GETTER,
						   "Setting getter of unassignable object");
	return;
}

inline void
InkWarn_Setting_Unassignable_Setter(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_SETTING_UNASSIGNABLE_SETTER,
						   "Setting setter of unassignable object");
	return;
}

inline void
InkWarn_Type_Name_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_TYPE_NAME_ARGUMENT_REQUIRE,
						   "Function typename requires at least 1 argument");
	return;
}

inline void
InkWarn_With_Attachment_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_WITH_ATTACHMENT_REQUIRE,
						   "with/expand attachment needs array as argument");
	return;
}

inline void
InkWarn_Failed_Open_File(Ink_InterpreteEngine *engine, const char *path)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FAILED_OPEN_FILE,
						   "Failed to open file $(path)", path);
	return;
}

inline void
InkWarn_Failed_Find_Mod(Ink_InterpreteEngine *engine, const char *path)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FAILED_FIND_MOD,
						   "Failed to open module directory $(path)", path);
	return;
}

inline void
InkWarn_Failed_Load_Mod(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FAILED_LOAD_MOD,
						   "Failed to load module $(name):", name);
	return;
}

inline void
InkWarn_Failed_Find_Loader(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FAILED_FIND_LOADER,
						   "Failed to find loader function in module $(name):", name);
	return;
}

inline void
InkWarn_Failed_Find_Init(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FAILED_FIND_INIT,
						   "Failed to find init function in module $(name):", name);
	return;
}

inline void
InkWarn_Failed_Init_Mod(Ink_InterpreteEngine *engine, int errnum)
{
	std::stringstream strm;
	strm << "Failed to initialize module, initializer returned error no: " << errnum;

	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FAILED_INIT_MOD,
						   strm.str().c_str());
	return;
}

inline void
InkWarn_Hash_Table_Mapping_Expect_String(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_HASH_TABLE_MAPPING_EXPECT_STRING,
						   "Hash table mapping expects string as argument");
	return;
}

inline void
InkWarn_Delete_Function_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_DELETE_FUNCTION_ARGUMENT_REQUIRE,
						   "Delete function requires assignable object");
	return;
}

inline void
InkWarn_Eval_Called_Without_Current_Engine(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_EVAL_CALLED_WITHOUT_CURRENT_ENGINE,
						   "Eval: No current engine created. stopped");
	return;
}

inline void
InkWarn_Invalid_Argument_For_String_Add(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_INVALID_ARGUMENT_FOR_STRING_ADD,
						   "Invalid argument of type <$(type)>, expecting numeric, string or object with to_str method",
						   getTypeName(engine, type));
	return;
}

inline void
InkWarn_Invalid_Return_Value_Of_To_String(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_INVALID_RETURN_VALUE_OF_TO_STRING,
						   "Invalid return value of type <$(type)> of to_str method, expecting string",
						   getTypeName(engine, type));
	return;
}

inline void
InkWarn_Load_Mod_On_Wrong_OS(Ink_InterpreteEngine *engine, const char *path)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_LOAD_MOD_ON_WRONG_OS,
						   "Cannot find module of correct system version in $(path)", path);
	return;
}

inline void
InkWarn_No_File_In_Mod(Ink_InterpreteEngine *engine, const char *path)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_NO_FILE_IN_MOD,
						   "No file contained in the module $(path)", path);
	return;
}

inline void
InkWarn_Actor_Conflict(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_ACTOR_CONFLICT,
						   "Conflict actor name \'$(name)\'", name);
	return;
}

inline void
InkWarn_Failed_Create_Process(Ink_InterpreteEngine *engine, int err_code)
{
	std::stringstream strm;
	strm << "Failed to create process, error code: " << err_code;
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FAILED_CREATE_PROCESS,
						   strm.str().c_str());
	return;
}

inline void
InkWarn_Circular_Prototype_Reference(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_CIRCULAR_PROTOTYPE_REFERENCE,
						   "Circular prototype reference detected");
	return;
}

inline void
InkWarn_String_Index_Exceed(Ink_InterpreteEngine *engine,
							wstring::size_type index,
							wstring::size_type size)
{
	std::stringstream strm;
	strm << "Index " << index << " exceed size of the string(" << size << ")";
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_ARRAY_INDEX_EXCEED,
						   strm.str().c_str());
	return;
}

inline void
InkWarn_Sub_String_Exceed(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_SUB_STRING_EXCEED,
						   "Substring exceed the original string");
	return;
}

inline void
InkWarn_Unreachable_Bonding(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_UNREACHABLE_BONDING,
						   "Unreachable bonding detected, breaking");
	return;
}

inline void
InkWarn_Divided_By_Zero(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_DIVIDED_BY_ZERO,
						   "Divided by zero");
	return;
}

inline void
InkWarn_Undefined_Custom_Interrupt_Name(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_UNDEFINED_CUSTOM_INTERRUPT_NAME,
						   "Undefined custom interrupt signal name \'$(name)\'",
						   name);
	return;
}

inline void
InkWarn_Unregistered_Interrupt_Signal(Ink_InterpreteEngine *engine, Ink_InterruptSignal sig)
{
	std::stringstream strm;
	strm << "Unregisterred interrupt signal: " << sig;
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_UNREGISTERED_INTERRUPT_SIGNAL,
						   strm.str().c_str());
	return;
}

inline void
InkWarn_Trapping_Untrapped_Signal(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_TRAPPING_UNTRAPPED_SIGNAL,
						   "Trapping untrapped signal \'$(name)\'",
						   name);
	return;
}

inline void
InkWarn_Package_Broken(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_PACKAGE_BROKEN,
						   "Package \'$(name)\' reported a broken sign",
						   name);
	return;
}

inline void
InkWarn_Failed_Create_Coroutine(Ink_InterpreteEngine *engine, int err_code)
{
	std::stringstream strm;
	strm << "Failed to create coroutine, error code: " << err_code;
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FAILED_CREATE_COROUTINE,
						   strm.str().c_str());
	return;
}

inline void
InkWarn_Cocall_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_COCALL_ARGUMENT_REQUIRE,
						   "cocall function need argument in form of (function, array, function, array, ...)");
	return;
}

inline void
InkWarn_Wrong_Ret_Val_For_Compare(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_WRONG_RET_VAL_FOR_COMPARE,
						   "Wrong return value of '<=>' method for comparision(expecting numeric, <type> given)",
						   getTypeName(engine, type));
	return;
}

inline void
InkWarn_Assign_Fixed(Ink_InterpreteEngine *engine, const char *name)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_ASSIGN_FIXED,
						   "Assigning fixed value '$(name)'", name);
	return;
}

inline void
InkWarn_Failed_Get_Constant(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FAILED_GET_CONSTANT,
						   "Failed to get constant value of object of type <$(type)>",
						   getTypeName(engine, type));
	return;
}

inline void
InkWarn_Fix_Require_Assignable_Argument(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WARN_FIX_REQUIRE_ASSIGNABLE_ARGUMENT,
						   "fix function require assignable argument");
	return;
}

inline void
InkWarn_Slice_Require_Numeric(Ink_InterpreteEngine *engine, Ink_ArgcType argno)
{
	std::stringstream strm;
	strm << "slice method require numeric argument(while the ";
	if (argno <= 3) {
		strm << (argno == 1 ? "first" : (argno == 2 ? "second" : "third"));
	} else {
		strm << argno << "th";
	}
	InkErro_doPrintWarning(engine, INK_EXCODE_WRAN_SLICE_REQUIRE_NUMERIC,
						   strm.str().c_str());
	return;
}

inline void
InkWarn_Slice_Require_Non_Zero_Range(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, INK_EXCODE_WRAN_SLICE_REQUIRE_NON_ZERO_RANGE,
						   "slice method require non-zero range(the third argument), set back to 1");
	return;
}

inline void
InkNote_Method_Fallthrough(Ink_InterpreteEngine *engine, const char *name, Ink_TypeTag origin, Ink_TypeTag to_type)
{
#ifdef INK_DEBUG_FLAG
	InkErro_doPrintNote(engine, "Method \'$(name)\' of object of type <$(origin)> fallthrough to that of type <$(to)>",
						name,
						getTypeName(engine, origin),
						getTypeName(engine, to_type));
#endif
	return;
}

inline void
InkNote_Method_Fallthrough(Ink_InterpreteEngine *engine, const char *from, const char *to,Ink_TypeTag type)
{
#ifdef INK_DEBUG_FLAG
	InkErro_doPrintNote(engine, "Method \'$(from)\' of object of type <$(type)> fallthrough to method \'$(to)\'",
						from, getTypeName(engine, type), to);
#endif
	return;
}

inline void
InkNote_Exceed_Argument(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintNote(engine, "Argument(s) exceed");
	return;
}

inline void
InkNote_Slice_Start_Greater(Ink_InterpreteEngine *engine,
							Ink_ArrayValue::size_type start,
							Ink_ArrayValue::size_type end)
{
	std::stringstream strm;
	strm << "The first argument "
		 << start << "(start index) is greater than the second argument "
		 << end << "(end index) in slice method(swap them in default)";
	InkErro_doPrintNote(engine, strm.str().c_str());
	return;
}

}

#endif
