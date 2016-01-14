#include "error.h"
#include "interface/engine.h"

int inkerr_current_line_number = -1;

void
InkErr_doPrintError(Ink_InterpreteEngine *engine, const char *msg)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->getFilePath()) ?
			tmp : "<unknown input>") << ": "
		 << "line " << inkerr_current_line_number << ": " << msg;
	
	cleanAll();
	ErrorMessage::popMessage(new ErrorInfo(ErrorInfo::Error, true, ErrorInfo::Exit1,
										   strm.str().c_str()));
	return;
}

void
InkErr_doPrintError(Ink_InterpreteEngine *engine, const char *msg, const char *arg1)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->getFilePath()) ?
			tmp : "<unknown input>") << ": "
		 << "line " << inkerr_current_line_number << ": " << msg;

	cleanAll();
	ErrorMessage::popMessage(new ErrorInfo(ErrorInfo::Error, true, ErrorInfo::Exit1,
										   strm.str().c_str(), arg1));
	return;
}

void
InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->getFilePath()) ?
			tmp : "<unknown input>") << ": "
		 << "line " << inkerr_current_line_number << ": " << msg;

	ErrorInfo *info = new ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									strm.str().c_str());
	ErrorMessage::popMessage(info);
	delete info;
	return;
}

void
InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg, const char *arg1)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->getFilePath()) ?
			tmp : "<unknown input>") << ": "
		 << "line " << inkerr_current_line_number << ": " << msg;

	ErrorInfo *info = new ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									strm.str().c_str(), arg1);
	ErrorMessage::popMessage(info);
	delete info;
	return;
}

void
InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg, const char *arg1, const char *arg2)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->getFilePath()) ?
			tmp : "<unknown input>") << ": "
		 << "line " << inkerr_current_line_number << ": " << msg;
	
	ErrorInfo *info = new ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									strm.str().c_str(), arg1, arg2);
	ErrorMessage::popMessage(info);
	delete info;
	return;
}