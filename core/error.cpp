#include <signal.h>
#include "object.h"
#include "error.h"
#include "exception.h"
#include "thread/actor.h"
#include "thread/thread.h"
#include "interface/engine.h"

namespace ink {

void
InkErr_doPrintError(Ink_InterpreteEngine *engine, Ink_ExceptionCode ex_code, const char *msg)
{
	stringstream strm;
	bool if_abort = (engine == NULL);
	const char *tmp;
	strm << (engine && (tmp = engine->current_file_name) ?
			tmp : "<unknown input>") << ": "
		 << "line " << (engine ? engine->current_line_number : -1) << ": " << msg;
	
	cleanAll(engine);
	
	ErrorMessage::popMessage(ErrorInfo(ErrorInfo::Error, true, if_abort ? ErrorInfo::Abort
																		: ErrorInfo::NoAct,
									   strm.str().c_str()));

	if (engine) {
		engine->printTrace(stderr, engine->getTrace(), "***INK EXCEPTION*** ");
		engine->setInterrupt(INTER_THROW,
							 new Ink_ExceptionMessage(engine, ex_code, engine->current_line_number, msg));
	}

	return;
}

void
InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->current_file_name) ?
			tmp : "<unknown input>") << ": "
		 << "line " << (engine ? engine->current_line_number : -1) << ": " << msg;

	ErrorMessage::popMessage(ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									   strm.str().c_str()));

	if (engine && engine->getErrorMode() == INK_ERRMODE_STRICT) {
		engine->printTrace(stderr, engine->getTrace(), "***INK EXCEPTION(strict mode)*** ");
		engine->setInterrupt(INTER_THROW,
							 new Ink_ExceptionMessage(engine, 0, engine->current_line_number, msg));
	}

	return;
}

void
InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg, const char *arg1)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->current_file_name) ?
			tmp : "<unknown input>") << ": "
		 << "line " << (engine ? engine->current_line_number : -1) << ": " << msg;

	ErrorMessage::popMessage(ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									   strm.str().c_str(), arg1));

	if (engine && engine->getErrorMode() == INK_ERRMODE_STRICT) {
		engine->printTrace(stderr, engine->getTrace(), "***INK EXCEPTION(strict mode)*** ");
		engine->setInterrupt(INTER_THROW,
							 new Ink_ExceptionMessage(engine, 0, engine->current_line_number, msg));
	}

	return;
}

void
InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg, const char *arg1, const char *arg2)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->current_file_name) ?
			tmp : "<unknown input>") << ": "
		 << "line " << (engine ? engine->current_line_number : -1) << ": " << msg;
	
	ErrorMessage::popMessage(ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									   strm.str().c_str(), arg1, arg2));

	if (engine && engine->getErrorMode() == INK_ERRMODE_STRICT) {
		engine->printTrace(stderr, engine->getTrace(), "***INK EXCEPTION(strict mode)*** ");
		engine->setInterrupt(INTER_THROW,
							 new Ink_ExceptionMessage(engine, 0, engine->current_line_number, msg));
	}

	return;
}

const char *getTypeName(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	return engine->getTypeName(type);
}

}
