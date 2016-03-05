#ifndef _DEBUG_H
#define _DEBUG_H

#include <string>
#include <stdio.h>
#include "type.h"
#include "hash.h"
#include "general.h"

#define DBG_TAB "    "
#define DBG_DEFAULT_PREFIX "INK_DEBUG: "
#define DBG_DEFAULT_MAX_TRACE 7

namespace ink {

class Ink_Object;
class Ink_ContextChain;

class DBG_TypeMapping {
public:
	Ink_TypeTag type;
	const char *name;
	Ink_Object *proto;
	std::string *name_p;

	DBG_TypeMapping(Ink_TypeTag type, const char *name, std::string *name_p = NULL)
	: type(type), name(name), name_p(name_p)
	{
		proto = NULL;
	}

	~DBG_TypeMapping()
	{
		if (name_p) delete name_p;
	}
};

struct DBG_FixedTypeMapping {
	Ink_TypeTag tag;
	const char *name;
};

struct DBG_NativeSignalMapping {
	Ink_InterruptSignal sig;
	const char *name;
};

const char *getNativeSignalName(Ink_InterruptSignal sig);

}

#endif
