#ifndef _DEBUG_H
#define _DEBUG_H

#include <string>
#include <stdio.h>
#include "type.h"

#define DBG_TAB "    "
#define DBG_DEFAULT_PREFIX "INK_DEBUG: "

class Ink_Object;
class Ink_ContextChain;

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
int DBG_registerType(const char *name);
void DBG_initPrintDebugInfo();
void DBG_printDebugInfo(FILE *fp, Ink_Object *obj, std::string prefix = DBG_DEFAULT_PREFIX,
						std::string slot_prefix = "", bool if_scan_slot = true);
inline void DBG_printDebugInfo(bool if_scan_slot, FILE *fp, Ink_Object *obj,
							   std::string prefix = DBG_DEFAULT_PREFIX, std::string slot_prefix = "")
{
	DBG_printDebugInfo(fp, obj, prefix, slot_prefix, if_scan_slot);
	return;
}
void DBG_printTrace(FILE *fp, Ink_ContextChain *context, std::string prefix = DBG_DEFAULT_PREFIX);

#endif
