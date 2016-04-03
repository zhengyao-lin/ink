#include <string>
#include <algorithm>
#include <ctype.h>
#include "object.h"
#include "context.h"
#include "exception.h"
#include "native/general.h"

namespace ink {

using namespace std;

static Ink_Object *Ink_Errno_Equal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);
static Ink_Object *Ink_Errno_NotEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);

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

	setSlot_c("==", new Ink_FunctionObject(engine, Ink_Errno_Equal));
	setSlot_c("!=", new Ink_FunctionObject(engine, Ink_Errno_NotEqual));
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
		mod_id = getInt(as<Ink_Numeric>(tmp_obj)->getValue());
	} else {
		return NULL;
	}

	if ((tmp_obj = ex->getSlot(ex->engine, "ex_code"))->type == INK_NUMERIC) {
		ex_code = getInt(as<Ink_Numeric>(tmp_obj)->getValue());
	} else {
		return NULL;
	}

	if ((tmp_obj = ex->getSlot(ex->engine, "file_name"))->type == INK_STRING) {
		file_name = as<Ink_String>(tmp_obj)->getValue();
	} else {
		return NULL;
	}

	if ((tmp_obj = ex->getSlot(ex->engine, "lineno"))->type == INK_NUMERIC) {
		lineno = getInt(as<Ink_Numeric>(tmp_obj)->getValue());
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

inline bool isErrnoEqual(Ink_InterpreteEngine *engine, Ink_Object *base, Ink_Object *ex)
{
	Ink_Object *tmp_obj1, *tmp_obj2;

	if ((tmp_obj1 = ex->getSlot(engine, "mod_id"))->type == INK_NUMERIC
		&& (tmp_obj2 = base->getSlot(engine, "mod_id"))->type == INK_NUMERIC) {
		if (as<Ink_Numeric>(tmp_obj1)->getValue()
			!= as<Ink_Numeric>(tmp_obj2)->getValue()) {
			return false;
		}
	} else {
		return false;
	}

	if ((tmp_obj1 = ex->getSlot(engine, "ex_code"))->type == INK_NUMERIC
		&& (tmp_obj2 = base->getSlot(engine, "ex_code"))->type == INK_NUMERIC) {
		if (as<Ink_Numeric>(tmp_obj1)->getValue()
			!= as<Ink_Numeric>(tmp_obj2)->getValue()) {
			return false;
		}
	} else {
		return false;
	}

	return true;
}

static Ink_Object *Ink_Errno_Equal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{

	if (!checkArgument(engine, argc, 1)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, isErrnoEqual(engine, base, argv[0]));
}

static Ink_Object *Ink_Errno_NotEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{

	if (!checkArgument(engine, argc, 1)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, !isErrnoEqual(engine, base, argv[0]));
}

static Ink_Object *Ink_Errno_Missing(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{

	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	string err_name = as<Ink_String>(argv[0])->getValue();
	char *tmp_str = Ink_mbstoupper_alloc(err_name.c_str());

	Ink_ErrnoObject *errno_obj = as<Ink_ErrnoObject>(base);
	Ink_SizeType i;
	Ink_Object *ret;

	for (i = 0; i < errno_obj->errno_map_size; i++) {
		if (!strcmp(tmp_str, errno_obj->errno_map[i].name)) {
			ret = new Ink_Object(engine);
			ret->setSlot_c("mod_id", new Ink_Numeric(engine, errno_obj->errno_map[i].mod_id));
			ret->setSlot_c("ex_code", new Ink_Numeric(engine, errno_obj->errno_map[i].ex_code));
			ret->setSlot_c("==", new Ink_FunctionObject(engine, Ink_Errno_Equal));
			ret->setSlot_c("!=", new Ink_FunctionObject(engine, Ink_Errno_NotEqual));

			free(tmp_str);

			return ret;
		}
	}

	free(tmp_str);

	return UNDEFINED;
}

Ink_ErrnoObject::Ink_ErrnoObject(Ink_InterpreteEngine *engine, Ink_SizeType size, Ink_ErrnoMap *map)
: Ink_Object(engine), errno_map_size(size), errno_map(map)
{
	setSlot_c("missing", new Ink_FunctionObject(engine, Ink_Errno_Missing));
}

Ink_Constant *Ink_ErrnoObject::toConstant(Ink_InterpreteEngine *engine)
{
	return new Ink_ErrnoConstant(errno_map_size, errno_map);
}

Ink_Object *Ink_ErrnoConstant::toObject(Ink_InterpreteEngine *engine)
{
	return new Ink_ErrnoObject(engine, errno_map_size, errno_map);
}

}
