#ifndef _CONSTANT_H_
#define _CONSTANT_H_

#include <string>
#include <map>
#include <assert.h>
#include "type.h"
#include "general.h"

namespace ink {

class Ink_Object;
class Ink_InterpreteEngine;

struct Ink_Constant {
	virtual Ink_Object *toObject(Ink_InterpreteEngine *engine)
	{
		assert(0);
		return NULL;
	}

	virtual ~Ink_Constant()
	{ }
};

struct Ink_NumericConstant: public Ink_Constant {
	Ink_NumericValue value;

	Ink_NumericConstant(Ink_NumericValue value)
	: value(value)
	{ }

	virtual Ink_Object *toObject(Ink_InterpreteEngine *engine);
};

struct Ink_StringConstant: public Ink_Constant {
	std::wstring *value;

	Ink_StringConstant(std::wstring value)
	: value(new std::wstring(value))
	{ }

	virtual Ink_Object *toObject(Ink_InterpreteEngine *engine);

	virtual ~Ink_StringConstant()
	{
		delete value;
	}
};

typedef std::map<std::wstring, Ink_Constant *> Ink_ConstantTable;

}

#endif
