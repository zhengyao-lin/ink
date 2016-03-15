#include <string>
#include "object.h"
#include "exception.h"

namespace ink {

using namespace std;

Ink_ExceptionMessage::Ink_ExceptionMessage(Ink_InterpreteEngine *engine, Ink_ModuleID mod_id,
										   Ink_ExceptionCode ex_code, const char *file_name,
										   Ink_LineNoType lineno, std::string msg)
: Ink_Object(engine)
{
	setSlot_c("mod_id", new Ink_Numeric(engine, mod_id));
	setSlot_c("ex_code", new Ink_Numeric(engine, ex_code));
	setSlot_c("file_name", new Ink_String(engine, std::string(file_name)));
	setSlot_c("lineno", new Ink_Numeric(engine, lineno));
	setSlot_c("msg", new Ink_String(engine, msg));
}

Ink_ExceptionRaw *Ink_ExceptionRaw::toRaw(Ink_Object *ex)
{
	if (!ex) return NULL;

	Ink_ModuleID mod_id;
	Ink_ExceptionCode ex_code;
	string file_name;
	Ink_LineNoType lineno;
	string msg;
	Ink_Object *tmp_obj;

	if ((tmp_obj = ex->getSlot(ex->engine, "mod_id"))->type == INK_NUMERIC) {
		mod_id = as<Ink_Numeric>(tmp_obj)->value;
	} else {
		return NULL;
	}

	if ((tmp_obj = ex->getSlot(ex->engine, "ex_code"))->type == INK_NUMERIC) {
		ex_code = as<Ink_Numeric>(tmp_obj)->value;
	} else {
		return NULL;
	}

	if ((tmp_obj = ex->getSlot(ex->engine, "file_name"))->type == INK_STRING) {
		file_name = as<Ink_String>(tmp_obj)->getValue();
	} else {
		return NULL;
	}

	if ((tmp_obj = ex->getSlot(ex->engine, "lineno"))->type == INK_NUMERIC) {
		lineno = as<Ink_Numeric>(tmp_obj)->value;
	} else {
		return NULL;
	}

	if ((tmp_obj = ex->getSlot(ex->engine, "msg"))->type == INK_STRING) {
		msg = as<Ink_String>(tmp_obj)->getValue();
	} else {
		return NULL;
	}

	return new Ink_ExceptionRaw(mod_id, ex_code, file_name.c_str(), lineno, msg);
}

}
