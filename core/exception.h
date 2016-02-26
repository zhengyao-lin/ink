#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include "object.h"
#include "general.h"

namespace ink {

class Ink_InterpreteEngine;

class Ink_ExceptionMessage: public Ink_Object {
public:

	Ink_ExceptionMessage(Ink_InterpreteEngine *engine,
						 Ink_ExceptionCode ex_code, const char *file_name,
						 Ink_LineNoType lineno, std::string msg)
	: Ink_Object(engine)
	{
		setSlot_c("ex_code", new Ink_Numeric(engine, ex_code));
		setSlot_c("file_name", new Ink_String(engine, std::string(file_name)));
		setSlot_c("lineno", new Ink_Numeric(engine, lineno));
		setSlot_c("msg", new Ink_String(engine, msg));
	}
};

struct Ink_ExceptionRaw {
	Ink_ExceptionCode ex_code;
	std::string *file_name;
	Ink_LineNoType lineno;
	std::string *msg;

	Ink_ExceptionRaw(Ink_ExceptionCode ex_code, const char *file_name,
					 Ink_LineNoType lineno, std::string msg)
	: ex_code(ex_code), file_name(new std::string(file_name)),
	  lineno(lineno), msg(new std::string(msg))
	{ }

	inline Ink_ExceptionMessage *toObject(Ink_InterpreteEngine *engine)
	{
		return new Ink_ExceptionMessage(engine, ex_code, file_name->c_str(), lineno, *msg);
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
