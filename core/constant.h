#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <string>
#include <map>
#include "type.h"
#include "general.h"

namespace ink {

class Ink_Object;
class Ink_InterpreteEngine;

struct Ink_Constant {
	Ink_TypeTag type;
	union {
		std::wstring *str;
		Ink_NumericValue num;
	};

	Ink_Constant(std::wstring str)
	: str(new std::wstring(str))
	{ type = INK_STRING; }

	Ink_Constant(Ink_NumericValue num)
	: num(num)
	{ type = INK_NUMERIC; }

	Ink_Object *toObject(Ink_InterpreteEngine *engine);
	static Ink_Constant *fromObject(Ink_Object *obj);

	~Ink_Constant()
	{
		if (type == INK_STRING)
			delete str;
	}
};

typedef std::map<std::wstring, Ink_Constant *> Ink_ConstantTable;

}

#endif
