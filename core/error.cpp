#include <signal.h>
#include "error.h"
#include "actor.h"
#include "thread/thread.h"
#include "interface/engine.h"

void
InkErr_doPrintError(Ink_InterpreteEngine *engine, const char *msg)
{
	stringstream strm;
	const char *tmp;
	bool is_root = InkActor_isRootActor(engine);
	strm << (engine && (tmp = engine->getFilePath()) ?
			tmp : "<unknown input>") << ": "
		 << "line " << (engine ? engine->current_line_number : -1) << ": " << msg;
	
	cleanAll(engine);
	ErrorMessage::popMessage(ErrorInfo(ErrorInfo::Error, true, is_root ? ErrorInfo::Abort
																	   : ErrorInfo::NoAct,
									   strm.str().c_str()));

	if (!is_root) {
		/*if (engine && engine->actor_argument) {
			Ink_ActorFunction_sub_Argument *tmp_arg = engine->actor_argument;
			
			tmp_arg->engine->top_level = Ink_ExpressionList();
			InkActor_setDeadActor(tmp_arg->engine);
			if (tmp_arg->argv)
				free(tmp_arg->argv);
			delete tmp_arg->engine;
			delete tmp_arg;
		}*/
		delete engine;
		pthread_exit(NULL);
	}

	return;
}

void
InkErr_doPrintError(Ink_InterpreteEngine *engine, const char *msg, const char *arg1)
{
	stringstream strm;
	const char *tmp;
	bool is_root = InkActor_isRootActor(engine);
	strm << (engine && (tmp = engine->getFilePath()) ?
			tmp : "<unknown input>") << ": "
		 << "line " << (engine ? engine->current_line_number : -1) << ": " << msg;

	cleanAll(engine);
	ErrorMessage::popMessage(ErrorInfo(ErrorInfo::Error, true, is_root ? ErrorInfo::Abort
																	   : ErrorInfo::NoAct,
									   strm.str().c_str(), arg1));
	if (!is_root) {
		/*if (engine && engine->actor_argument) {
			Ink_ActorFunction_sub_Argument *tmp_arg = engine->actor_argument;
			
			tmp_arg->engine->top_level = Ink_ExpressionList();
			InkActor_setDeadActor(tmp_arg->engine);
			if (tmp_arg->argv)
				free(tmp_arg->argv);
			delete tmp_arg->engine;
			delete tmp_arg;
		}*/
		delete engine;
		pthread_exit(NULL);
	}

	return;
}

void
InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->getFilePath()) ?
			tmp : "<unknown input>") << ": "
		 << "line " << (engine ? engine->current_line_number : -1) << ": " << msg;

	ErrorMessage::popMessage(ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									   strm.str().c_str()));
	return;
}

void
InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg, const char *arg1)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->getFilePath()) ?
			tmp : "<unknown input>") << ": "
		 << "line " << (engine ? engine->current_line_number : -1) << ": " << msg;

	ErrorMessage::popMessage(ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									   strm.str().c_str(), arg1));
	return;
}

void
InkWarn_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg, const char *arg1, const char *arg2)
{
	stringstream strm;
	const char *tmp;
	strm << (engine && (tmp = engine->getFilePath()) ?
			tmp : "<unknown input>") << ": "
		 << "line " << (engine ? engine->current_line_number : -1) << ": " << msg;
	
	ErrorMessage::popMessage(ErrorInfo(ErrorInfo::Warning, true, ErrorInfo::NoAct,
									   strm.str().c_str(), arg1, arg2));
	return;
}

const char *getTypeName(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	return engine->getTypeName(type);
}