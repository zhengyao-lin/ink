#ifndef _DEBUG_H
#define _DEBUG_H

#include <string>
#include <stdio.h>
#include "type.h"

#define DBG_TAB "    "

class Ink_Object;

class DBG_TypeMapping {
public:
	int type;
	const char *friendly_name;

	DBG_TypeMapping(int type, const char *name)
	: type(type), friendly_name(name)
	{ }
};

extern DBG_TypeMapping *dbg_type_mapping;

inline const char *getTypeName(int type_tag)
{
	return dbg_type_mapping[type_tag].friendly_name;
}

void DBG_initTypeMapping();
void DBG_disposeTypeMapping();
int registerType(const char *name);
void initPrintDebugInfo();
void printDebugInfo(FILE *fp, Ink_Object *obj, std::string prefix = "INK_DBG: ", std::string slot_prefix = "");

#endif