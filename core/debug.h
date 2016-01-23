#ifndef _DEBUG_H
#define _DEBUG_H

#include <string>
#include <stdio.h>
#include "type.h"

#define DBG_TAB "    "
#define DBG_DEFAULT_PREFIX "INK_DEBUG: "

class Ink_Object;
class Ink_ContextChain;

typedef long DBG_CustomTypeType;

class DBG_TypeMapping {
public:
	DBG_CustomTypeType type;
	const char *friendly_name;

	DBG_TypeMapping(DBG_CustomTypeType type, const char *name)
	: type(type), friendly_name(name)
	{ }
};

struct DBG_FixedTypeMapping {
	Ink_TypeTag tag;
	const char *name;
};

#endif
