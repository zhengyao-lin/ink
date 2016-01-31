#ifndef _DEBUG_H
#define _DEBUG_H

#include <string>
#include <stdio.h>
#include "type.h"

#define DBG_TAB "    "
#define DBG_DEFAULT_PREFIX "INK_DEBUG: "

namespace ink {

class Ink_Object;
class Ink_ContextChain;

class DBG_TypeMapping {
public:
	Ink_TypeTag type;
	const char *friendly_name;

	DBG_TypeMapping(Ink_TypeTag type, const char *name)
	: type(type), friendly_name(name)
	{ }
};

struct DBG_FixedTypeMapping {
	Ink_TypeTag tag;
	const char *name;
};

}

#endif
