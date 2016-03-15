#include <signal.h>
#include "object.h"
#include "error.h"
#include "general.h"
#include "exception.h"
#include "thread/actor.h"
#include "thread/thread.h"
#include "interface/engine.h"

namespace ink {

void
InkErro_doPrintError_c(Ink_InterpreteEngine *engine, Ink_ModuleID mod_id, Ink_ExceptionCode ex_code, const char *msg, va_list args)
{
	const char *tmp;
	bool if_exit = (engine == NULL);
	const char *file_name = engine && (tmp = engine->current_file_name) ? tmp : "<unknown input>";
	Ink_LineNoType line_number = engine ? engine->current_line_number : -1;

	if (if_exit) {
		Ink_disposeEnv();
	}
	
	Ink_ErrorMessage err_msg = Ink_ErrorMessage(file_name, line_number, msg, args);
	err_msg.popWith(Ink_ErrorMessage::INK_ERR_LEVEL_ERROR,
					if_exit ? Ink_ErrorMessage::INK_ERR_ACTION_EXIT1
					  		: Ink_ErrorMessage::INK_ERR_ACTION_NONE);

	if (engine) {
		engine->printTrace(stderr, engine->getTrace(), "***INK EXCEPTION*** ");
		engine->setInterrupt(INTER_THROW, new Ink_ExceptionMessage(engine, mod_id, ex_code, file_name,
																   line_number, *(err_msg.message)));
	}

	return;
}

void
InkErro_doPrintWarning_c(Ink_InterpreteEngine *engine, Ink_ModuleID mod_id, Ink_ExceptionCode ex_code, const char *msg, va_list args)
{
	const char *tmp;
	const char *file_name = engine && (tmp = engine->current_file_name) ? tmp : "<unknown input>";
	Ink_LineNoType line_number = engine ? engine->current_line_number : -1;
	
	Ink_ErrorMessage err_msg(file_name, line_number, msg, args);
	err_msg.popWith(Ink_ErrorMessage::INK_ERR_LEVEL_WARNING);

	if (engine && engine->getErrorMode() == INK_ERRMODE_STRICT) {
		engine->printTrace(stderr, engine->getTrace(), "***INK EXCEPTION(strict mode)*** ");
		engine->setInterrupt(INTER_THROW, new Ink_ExceptionMessage(engine, mod_id, ex_code, file_name,
																   line_number, *(err_msg.message)));
	}

	return;
}

void
InkErro_doPrintNote_c(Ink_InterpreteEngine *engine, const char *msg, va_list args)
{
	const char *tmp;
	const char *file_name = engine && (tmp = engine->current_file_name) ? tmp : "<unknown input>";
	Ink_LineNoType line_number = engine ? engine->current_line_number : -1;
	
	Ink_ErrorMessage err_msg(file_name, line_number, msg, args);
	err_msg.popWith(Ink_ErrorMessage::INK_ERR_LEVEL_NOTE);

	return;
}

void
InkErro_doPrintError(Ink_InterpreteEngine *engine, const char *msg, ...)
{
	va_list args;
	
	va_start(args, msg);
	InkErro_doPrintError_c(engine, INK_CORE_MOD_ID, INK_EXCODE_UNDEFINED, msg, args);
	va_end(args);

	return;
}

void
InkErro_doPrintError(Ink_InterpreteEngine *engine, Ink_ExceptionCode ex_code, const char *msg, ...)
{
	va_list args;
	
	va_start(args, msg);
	InkErro_doPrintError_c(engine, INK_CORE_MOD_ID, ex_code, msg, args);
	va_end(args);

	return;
}

void
InkErro_doPrintError(Ink_InterpreteEngine *engine, Ink_ModuleID mod_id, Ink_ExceptionCode ex_code, const char *msg, ...)
{
	va_list args;
	
	va_start(args, msg);
	InkErro_doPrintError_c(engine, mod_id, ex_code, msg, args);
	va_end(args);

	return;
}

void
InkErro_doPrintWarning(Ink_InterpreteEngine *engine, const char *msg, ...)
{
	va_list args;
	
	va_start(args, msg);
	InkErro_doPrintWarning_c(engine, INK_CORE_MOD_ID, INK_EXCODE_UNDEFINED, msg, args);
	va_end(args);

	return;
}

void
InkErro_doPrintWarning(Ink_InterpreteEngine *engine, Ink_ExceptionCode ex_code, const char *msg, ...)
{
	va_list args;
	
	va_start(args, msg);
	InkErro_doPrintWarning_c(engine, INK_CORE_MOD_ID, ex_code, msg, args);
	va_end(args);

	return;
}

void
InkErro_doPrintWarning(Ink_InterpreteEngine *engine, Ink_ModuleID mod_id, Ink_ExceptionCode ex_code, const char *msg, ...)
{
	va_list args;
	
	va_start(args, msg);
	InkErro_doPrintWarning_c(engine, mod_id, ex_code, msg, args);
	va_end(args);

	return;
}

void
InkErro_doPrintNote(Ink_InterpreteEngine *engine, const char *msg, ...)
{
	va_list args;
	
	va_start(args, msg);
	InkErro_doPrintNote_c(engine, msg, args);
	va_end(args);

	return;
}

const char *getTypeName(Ink_InterpreteEngine *engine, Ink_TypeTag type)
{
	return engine->getTypeName(type);
}

}
