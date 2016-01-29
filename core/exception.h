#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "object.h"
#include "general.h"
#include "interface/engine.h"

namespace ink {

class Ink_ExceptionMessage: public Ink_Object {
public:

	Ink_ExceptionMessage(Ink_InterpreteEngine *engine,
						 Ink_ExceptionCode ex_code,
						 Ink_LineNoType lineno, const char *msg)
	: Ink_Object(engine)
	{
		setSlot("ex_code", new Ink_Numeric(engine, ex_code));
		setSlot("lineno", new Ink_Numeric(engine, lineno));
		setSlot("msg", new Ink_String(engine, string(msg)));
	} 
};

}

#endif
