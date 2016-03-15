#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include "object.h"
#include "general.h"

namespace ink {

class Ink_InterpreteEngine;
class Ink_Object;

class Ink_ExceptionMessage: public Ink_Object {
public:
	Ink_ExceptionMessage(Ink_InterpreteEngine *engine, Ink_ModuleID mod_id,
						 Ink_ExceptionCode ex_code, const char *file_name,
						 Ink_LineNoType lineno, std::string msg);
};

struct Ink_ExceptionRaw {
	Ink_ModuleID mod_id;
	Ink_ExceptionCode ex_code;
	std::string *file_name;
	Ink_LineNoType lineno;
	std::string *msg;

	Ink_ExceptionRaw(Ink_ModuleID mod_id, Ink_ExceptionCode ex_code,
					 const char *file_name, Ink_LineNoType lineno,
					 std::string msg)
	: mod_id(mod_id), ex_code(ex_code), file_name(new std::string(file_name)),
	  lineno(lineno), msg(new std::string(msg))
	{ }

	inline Ink_ExceptionMessage *toObject(Ink_InterpreteEngine *engine)
	{
		return new Ink_ExceptionMessage(engine, mod_id, ex_code, file_name->c_str(), lineno, *msg);
	}

	static Ink_ExceptionRaw *toRaw(Ink_Object *ex);

	~Ink_ExceptionRaw()
	{
		delete file_name;
		delete msg;
	}
};

}

#endif
