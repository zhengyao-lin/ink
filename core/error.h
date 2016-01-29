#ifndef _ERROR_H_
#define _ERROR_H_

#include "msg/emcore.h"
#include "type.h"
#include "debug.h"
#include "general.h"

namespace ink {

class Ink_ContextChain;
class Ink_InterpreteEngine;

void cleanAll(Ink_InterpreteEngine *engine);

void InkErr_doPrintError(Ink_InterpreteEngine *engine, const char *msg);
void InkErr_doPrintError(Ink_InterpreteEngine *engine, const char *msg, const char *arg1);

void InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg);
void InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg, const char *arg1);
void InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg, const char *arg1, const char *arg2);

const char *getTypeName(Ink_InterpreteEngine *engine, Ink_TypeTag type);

inline void
InkErr_Calling_Non_Function_Object(Ink_InterpreteEngine *engine)
{
	InkErr_doPrintError(engine, "Calling non-function object");
	return;
}

inline void
InkErr_Calling_Undefined_Object(Ink_InterpreteEngine *engine)
{
	InkErr_doPrintError(engine, "Calling undefined object");
	return;
}

inline void
InkErr_Yield_Without_Coroutine(Ink_InterpreteEngine *engine)
{
	InkErr_doPrintError(engine, "Function yield, but no coroutine is created");
	return;
}

inline void
InkErr_Failed_Open_File(Ink_InterpreteEngine *engine, const char *path)
{
	InkErr_doPrintError(engine, "Failed to open file $(path)", path);
	return;
}

inline void
InkWarn_Assigning_Unassignable_Expression(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Assigning unassignable expression");
	return;
}

inline void
InkWarn_Hash_not_found(Ink_InterpreteEngine *engine, const char *index)
{
	InkWarn_doPrintWarning(engine, "Creating undefined slot \'$(index)\'", index);
	return;
}

inline void
InkWarn_Unfit_Argument(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Unfit argument(s)");
	return;
}

inline void
InkWarn_Get_Undefined_Hash(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Getting hash of undefined value");
	return;
}

inline void
InkWarn_Insert_Non_Function_Object(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Inserting non-function object or with a non-lazy-expression argument");
	return;
}

inline void
InkWarn_Index_Exceed(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Index exceed size of the array");
	return;
}

inline void
InkWarn_Bonding_Failed(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Failed to bond");
	return;
}

inline void
InkWarn_Self_Bonding(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Self bonding detected(may cause dead loop)");
	return;
}

inline void
InkWarn_Get_Non_Array_Index(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Getting element of non-array");
	return;
}

inline void
InkWarn_Require_Lazy_Expression(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Loop condition expression requires lazy expression(or equivalent inline function)");
	return;
}

inline void
InkWarn_If_Argument_Fault(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "If function need at least 1 argument");
	return;
}

inline void
InkWarn_Function_Range_Call_Argument_Error(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Function range call requires at least 1 argument");
	return;
}

inline void
InkWarn_Function_Non_Range_Call(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Function range call requires argument of range type");
	return;
}

inline void
InkWarn_Incorrect_Range_Type(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Incorrect range type");
	return;
}

inline void
InkWarn_While_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "While function requires at least 1 argument");
	return;
}

inline void
InkWarn_While_Block_Require(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "While function requires the second argument as a block");
	return;
}

inline void
InkWarn_Get_Non_Function_Exp(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Getting expressions of non-function object");
	return;
}

inline void
InkWarn_Rebuild_Non_Array(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Rebuilding non-array object");
	return;
}

inline void
InkWarn_Invalid_Element_For_Rebuild(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Invalid element for rebuilding");
	return;
}

inline void
InkWarn_Remove_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Remove method need at least one numeric argument");
	return;
}

inline void
InkWarn_Too_Huge_Index(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Index greater than size");
	return;
}

inline void
InkWarn_Each_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Each function requires at least one function argument");
	return;
}

inline void
InkWarn_Failed_Finding_Method(Ink_InterpreteEngine *engine, const char *name)
{
	InkWarn_doPrintWarning(engine, "Failed to find method $(name)", name);
	return;
}

inline void
InkWarn_Wrong_Type(Ink_InterpreteEngine *engine, Ink_TypeTag expect, Ink_TypeTag type)
{
	stringstream strm;
	strm << "Expecting type <" << getTypeName(engine, expect)
		 << ">, <" << getTypeName(engine, type) << "> offered";
	InkWarn_doPrintWarning(engine, strm.str().c_str());
	return;
}

inline void
InkWarn_Too_Less_Argument(Ink_InterpreteEngine *engine, Ink_ArgcType min, Ink_ArgcType argc)
{
	stringstream strm;
	strm << "Too less argument. need at least " << min
		 << ", " << argc << " given";

	InkWarn_doPrintWarning(engine, strm.str().c_str());
	return;
}

inline void
InkWarn_Wrong_Argument_Type(Ink_InterpreteEngine *engine, Ink_TypeTag expect, Ink_TypeTag type)
{
	stringstream strm;
	strm << "Expecting object of type <" << getTypeName(engine, expect)
		 << ", that of <" << getTypeName(engine, type) << " offered";
	InkWarn_doPrintWarning(engine, strm.str().c_str());
	return;
}

inline void
InkWarn_Method_Fallthrough(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	stringstream strm;
	strm << "Method fallthrough to " << getTypeName(engine, type);
	InkWarn_doPrintWarning(engine, strm.str().c_str());
	return;
}

inline void
InkWarn_Not_Package(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Object is not a package(with load method)");
	return;
}

inline void
InkWarn_Setting_Unassignable_Getter(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Setting getter of unassignable object");
	return;
}

inline void
InkWarn_Setting_Unassignable_Setter(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Setting setter of unassignable object");
	return;
}

inline void
InkWarn_Type_Name_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Function typename requires at least 1 argument");
	return;
}

inline void
InkWarn_With_Attachment_Require(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "with attachment needs array as argument");
	return;
}

inline void
InkWarn_Failed_Open_File(Ink_InterpreteEngine *engine, const char *path)
{
	InkWarn_doPrintWarning(engine, "Failed to open file $(path)", path);
	return;
}

inline void
InkWarn_Failed_Find_Mod(Ink_InterpreteEngine *engine, const char *path)
{
	InkWarn_doPrintWarning(engine, "Failed to open module directory $(path)", path);
	return;
}

inline void
InkWarn_Failed_Load_Mod(Ink_InterpreteEngine *engine, const char *name)
{
	InkWarn_doPrintWarning(engine, "Failed to load module $(name):", name);
	return;
}

inline void
InkWarn_Failed_Find_Loader(Ink_InterpreteEngine *engine, const char *name)
{
	InkWarn_doPrintWarning(engine, "Failed to find loader function in module $(name):", name);
	return;
}

inline void
InkWarn_Failed_Find_Init(Ink_InterpreteEngine *engine, const char *name)
{
	InkWarn_doPrintWarning(engine, "Failed to find init function in module $(name):", name);
	return;
}

inline void
InkWarn_Failed_Init_Mod(Ink_InterpreteEngine *engine, int errnum)
{
	stringstream strm;
	strm << "Failed to initialize module, initializer returned error no: " << errnum;

	InkWarn_doPrintWarning(engine, strm.str().c_str());
	return;
}

inline void
InkWarn_Hash_Table_Mapping_Expect_String(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Hash table mapping expects string as argument");
	return;
}

inline void
InkWarn_Delete_Function_Argument_Require(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Delete function require at least one argument");
	return;
}

inline void
InkWarn_Eval_Called_Without_Current_Engine(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Eval: No current engine created. stopped");
	return;
}

inline void
InkWarn_Invalid_Argument_For_String_Add(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	stringstream strm;
	strm << "Invalid argument of type \'" << getTypeName(engine, type)
		 << "\', expecting numeric, string or object with to_str method";
	InkWarn_doPrintWarning(engine, strm.str().c_str());
	return;
}

inline void
InkWarn_Invalid_Return_Value_Of_To_String(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	stringstream strm;
	strm << "Invalid return value of type \'" << getTypeName(engine, type)
		 << "\' of to_str method, expecting string";
	InkWarn_doPrintWarning(engine, strm.str().c_str());
	return;
}

inline void
InkWarn_Load_Mod_On_Wrong_OS(Ink_InterpreteEngine *engine, const char *path)
{
	InkWarn_doPrintWarning(engine, "Loading module of wrong system version($(path))", path);
	return;
}

inline void
InkWarn_If_End_With_Else(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "if function ended with else");
	return;
}

inline void
InkWarn_If_End_With_Else_If(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "if function ended with else if(requires condition and block)");
	return;
}

inline void
InkWarn_If_End_With_Else_If_Has_Condition(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "if function ended with else if(requires block)");
	return;
}

inline void
InkWarn_Else_If_Has_No_Condition(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Condition for else if missing");
	return;
}

inline void
InkWarn_Actor_Conflict(Ink_InterpreteEngine *engine, const char *name)
{
	InkWarn_doPrintWarning(engine, "Conflict actor name \'$(name)\'", name);
	return;
}

inline void
InkWarn_Failed_Create_Process(Ink_InterpreteEngine *engine, int err_code)
{
	stringstream strm;
	strm << "Failed to create process, error code: " << err_code;
	InkWarn_doPrintWarning(engine, strm.str().c_str());
	return;
}

inline void
InkWarn_Circular_Prototype_Reference(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Circular prototype reference detected");
	return;
}

inline void
InkWarn_String_Index_Exceed(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "String index exceed length of the string");
	return;
}

inline void
InkWarn_Sub_String_Exceed(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Substring exceed the original string");
	return;
}

inline void
InkWarn_Unreachable_Bonding(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Unreachable bonding detected, breaking");
	return;
}

inline void
InkWarn_Divided_By_Zero(Ink_InterpreteEngine *engine)
{
	InkWarn_doPrintWarning(engine, "Divided by zero");
	return;
}

}

#endif
