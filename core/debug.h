#ifndef _DEBUG_H
#define _DEBUG_H

#include "type.h"

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

#endif