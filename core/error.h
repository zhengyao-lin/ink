#ifndef _ERROR_H_
#define _ERROR_H_

#include "../msg/emcore.h"

class Ink_ContextChain;

void cleanAll(Ink_ContextChain *context);
extern int inkerr_current_line_number;

inline void
InkErr_doPrintError(Ink_ContextChain *context, const char *msg)
{
	stringstream strm;
	strm << "line " << inkerr_current_line_number << ": " << msg;
	
	cleanAll(context);
	ErrorMessage::popMessage(new ErrorInfo(ErrorInfo::Error, true, ErrorInfo::Exit1,
										   strm.str().c_str()));
	return;
}

inline void
InkErr_doPrintError(Ink_ContextChain *context, const char *msg, const char *arg1)
{
	stringstream strm;
	strm << "line " << inkerr_current_line_number << ": " << msg;

	cleanAll(context);
	ErrorMessage::popMessage(new ErrorInfo(ErrorInfo::Error, true, ErrorInfo::Exit1,
										   strm.str().c_str(), arg1));
	return;
}

inline void
InkWarn_doPrintWarning(const char *msg)
{
	stringstream strm;
	strm << "line " << inkerr_current_line_number << ": " << msg;

	ErrorInfo *info = new ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct, strm.str().c_str());
	ErrorMessage::popMessage(info);
	delete info;
	return;
}

inline void
InkWarn_doPrintWarning(const char *msg, const char *arg1)
{
	ErrorInfo *info = new ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct, msg, arg1);
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
InkWarn_Get_Null_Hash()
{
	InkWarn_doPrintWarning("Getting the hash of null or undefined(cast to normal object)");
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

#endif