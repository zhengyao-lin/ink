#ifndef _ERROR_H_
#define _ERROR_H_

#include "../msg/emcore.h"

class Ink_ContextChain;

void cleanAll(Ink_ContextChain *context);

inline void
InkErr_doPrintError(Ink_ContextChain *context, const char *msg)
{
	cleanAll(context);
	ErrorMessage::popMessage(new ErrorInfo(ErrorInfo::Error, true, ErrorInfo::Exit1,
										   msg));
	return;
}

inline void
InkErr_doPrintError(Ink_ContextChain *context, const char *msg, const char *arg1)
{
	cleanAll(context);
	ErrorMessage::popMessage(new ErrorInfo(ErrorInfo::Error, true, ErrorInfo::Exit1,
										   msg, arg1));
	return;
}

inline void
InkWarn_doPrintWarning(const char *msg)
{
	ErrorInfo *info = new ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct, msg);
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
InkWarn_Hash_not_found(const char *index)
{
	InkWarn_doPrintWarning("Creating undefined slot \'$(index)\'", index);
	return;
}

#endif