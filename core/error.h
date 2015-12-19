#ifndef _ERROR_H_
#define _ERROR_H_

#include "../msg/emcore.h"
#include "type.h"
#include "debug.h"

class Ink_ContextChain;

void cleanAll();
extern int inkerr_current_line_number;

inline void
InkErr_doPrintError(Ink_ContextChain *context, const char *msg)
{
	stringstream strm;
	strm << "line " << inkerr_current_line_number << ": " << msg;
	
	cleanAll();
	ErrorMessage::popMessage(new ErrorInfo(ErrorInfo::Error, true, ErrorInfo::Exit1,
										   strm.str().c_str()));
	return;
}

inline void
InkErr_doPrintError(Ink_ContextChain *context, const char *msg, const char *arg1)
{
	stringstream strm;
	strm << "line " << inkerr_current_line_number << ": " << msg;

	cleanAll();
	ErrorMessage::popMessage(new ErrorInfo(ErrorInfo::Error, true, ErrorInfo::Exit1,
										   strm.str().c_str(), arg1));
	return;
}

inline void
InkWarn_doPrintWarning(const char *msg)
{
	stringstream strm;
	strm << "line " << inkerr_current_line_number << ": " << msg;

	ErrorInfo *info = new ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									strm.str().c_str());
	ErrorMessage::popMessage(info);
	delete info;
	return;
}

inline void
InkWarn_doPrintWarning(const char *msg, const char *arg1)
{
	stringstream strm;
	strm << "line " << inkerr_current_line_number << ": " << msg;

	ErrorInfo *info = new ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									strm.str().c_str(), arg1);
	ErrorMessage::popMessage(info);
	delete info;
	return;
}

inline void
InkWarn_doPrintWarning(const char *msg, const char *arg1, const char *arg2)
{
	stringstream strm;
	strm << "line " << inkerr_current_line_number << ": " << msg;
	
	ErrorInfo *info = new ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									strm.str().c_str(), arg1, arg2);
	ErrorMessage::popMessage(info);
	delete info;
	return;
}

inline void
InkErr_Assigning_Unassignable_Expression(Ink_ContextChain *context)
{
	InkErr_doPrintError(context, "Assigning unassignable expression");
	return;
}

inline void
InkErr_Calling_Non_Function_Object(Ink_ContextChain *context)
{
	InkErr_doPrintError(context, "Calling non-function object");
	return;
}

inline void
InkErr_Calling_Undefined_Object(Ink_ContextChain *context)
{
	InkErr_doPrintError(context, "Calling undefined object");
	return;
}

inline void
InkErr_Missing_Semicolon()
{
	InkErr_doPrintError(NULL, "Missing semicolon");
	return;
}

inline void
InkErr_Failed_Open_File(const char *path)
{
	InkErr_doPrintError(NULL, "Failed to open file $(path)", path);
	return;
}

inline void
InkWarn_Hash_not_found(const char *index)
{
	InkWarn_doPrintWarning("Creating undefined slot \'$(index)\'", index);
	return;
}

inline void
InkWarn_Unfit_Argument()
{
	InkWarn_doPrintWarning("Unfit argument(s)");
	return;
}

inline void
InkWarn_Get_Undefined_Hash()
{
	InkWarn_doPrintWarning("Getting hash of undefined value");
	return;
}

inline void
InkWarn_Insert_Non_Function_Object()
{
	InkWarn_doPrintWarning("Inserting non-function object or with a non-lazy-expression argument");
	return;
}

inline void
InkWarn_Index_Exceed()
{
	InkWarn_doPrintWarning("Index exceed size of the array");
	return;
}

inline void
InkWarn_Bonding_Failed()
{
	InkWarn_doPrintWarning("Failed to bond");
	return;
}

inline void
InkWarn_Self_Bonding()
{
	InkWarn_doPrintWarning("Self bonding detected(may cause dead loop)");
	return;
}

inline void
InkWarn_Get_Non_Array_Index()
{
	InkWarn_doPrintWarning("Getting element of non-array");
	return;
}

inline void
InkWarn_Require_Lazy_Expression()
{
	InkWarn_doPrintWarning("Loop condition expression requires lazy expression(or equivalent inline function)");
	return;
}

inline void
InkWarn_If_Argument_Fault()
{
	InkWarn_doPrintWarning("If function need at least 2 arguments");
	return;
}

inline void
InkWarn_Function_Range_Call_Argument_Error()
{
	InkWarn_doPrintWarning("Function range call requires at least 1 argument");
	return;
}

inline void
InkWarn_Function_Non_Range_Call()
{
	InkWarn_doPrintWarning("Function range call requires argument of range type");
	return;
}

inline void
InkWarn_Incorrect_Range_Type()
{
	InkWarn_doPrintWarning("Incorrect range type");
	return;
}

inline void
InkWarn_While_Argument_Require()
{
	InkWarn_doPrintWarning("While function requires at least 2 arguments");
	return;
}

inline void
InkWarn_While_Block_Require()
{
	InkWarn_doPrintWarning("While function requires the second argument as a block");
	return;
}

inline void
InkWarn_Get_Non_Function_Exp()
{
	InkWarn_doPrintWarning("Getting expressions of non-function object");
	return;
}

inline void
InkWarn_Rebuild_Non_Array()
{
	InkWarn_doPrintWarning("Rebuilding non-array object");
	return;
}

inline void
InkWarn_Invalid_Element_For_Rebuild()
{
	InkWarn_doPrintWarning("Invalid element for rebuilding");
	return;
}

inline void
InkWarn_Remove_Argument_Require()
{
	InkWarn_doPrintWarning("Remove method need at least one numeric argument");
	return;
}

inline void
InkWarn_Too_Huge_Index()
{
	InkWarn_doPrintWarning("Index greater than size");
	return;
}

inline void
InkWarn_Each_Argument_Require()
{
	InkWarn_doPrintWarning("Each function requires at least one function argument");
	return;
}

inline void
InkWarn_Failed_Finding_Method(const char *name)
{
	InkWarn_doPrintWarning("Failed to find method $(name)", name);
	return;
}

enum Ink_TypeTag;

inline void
InkWarn_Wrong_Type(Ink_TypeTag expect, Ink_TypeTag type)
{
	InkWarn_doPrintWarning("Expecting type <$(expect)>, <$(type)> offered",
						   getTypeName(expect), getTypeName(type));
	return;
}

inline void
InkWarn_Too_Less_Argument(unsigned int min, unsigned int argc)
{
	stringstream strm;
	strm << "Too less argument. need at least " << min
		 << ", " << argc << " given";

	InkWarn_doPrintWarning(strm.str().c_str());
	return;
}

inline void
InkWarn_Wrong_Argument_Type(Ink_TypeTag expect, Ink_TypeTag type)
{
	InkWarn_doPrintWarning("Expecting object of type <$(expect)>, that of <$(type)> offered",
						   getTypeName(expect), getTypeName(type));
	return;
}

inline void
InkWarn_Method_Fallthrough(Ink_TypeTag type)
{
	InkWarn_doPrintWarning("Method fallthrough to $(type)",
						   getTypeName(type));
	return;
}

inline void
InkWarn_Not_Package()
{
	InkWarn_doPrintWarning("Object is not a package(with load method)");
	return;
}

inline void
InkWarn_Setting_Unassignable_Getter()
{
	InkWarn_doPrintWarning("Setting getter of unassignable object");
	return;
}

inline void
InkWarn_Setting_Unassignable_Setter()
{
	InkWarn_doPrintWarning("Setting setter of unassignable object");
	return;
}

inline void
InkWarn_Type_Name_Argument_Require()
{
	InkWarn_doPrintWarning("Function typename requires at least 1 argument");
	return;
}

#endif