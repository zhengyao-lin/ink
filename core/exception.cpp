#include <string>
#include "object.h"
#include "exception.h"

namespace ink {

using namespace std;

Ink_ExceptionRaw *Ink_ExceptionRaw::toRaw(Ink_Object *ex)
{
	if (!ex) return NULL;

	Ink_ExceptionCode ex_code;
	string file_name;
	Ink_LineNoType lineno;
	string msg;
	Ink_Object *tmp_obj;

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

	return new Ink_ExceptionRaw(ex_code, file_name.c_str(), lineno, msg);
}

}
