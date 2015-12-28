#include <stdlib.h>
#include "debug.h"
#include "object.h"
#include "general.h"
#include "type.h"

int dbg_type_mapping_length = 0;
DBG_TypeMapping *dbg_type_mapping = NULL;
struct TempMapping {
	Ink_TypeTag tag;
	const char *name;
} dbg_fixed_type_mapping[] = 
{
	{ INK_NULL,			"null" },
	{ INK_UNDEFINED,	"undefined" },
	{ INK_OBJECT,		"object" },
	{ INK_NUMERIC,		"numeric" },
	{ INK_STRING,		"string" },
	{ INK_CONTEXT,		"context" },
	{ INK_FUNCTION,		"function" },
	{ INK_ARRAY,		"array" },
	{ INK_UNKNOWN,		"unknown" }
};

void DBG_initTypeMapping()
{
	int i;

	dbg_type_mapping_length = INK_LAST;
	dbg_type_mapping = (DBG_TypeMapping *)malloc(sizeof(DBG_TypeMapping) * dbg_type_mapping_length);

	for (i = 0; i < INK_LAST; i++) {
		dbg_type_mapping[i] = DBG_TypeMapping(i, dbg_fixed_type_mapping[i].name);
	}

	return;
}

int registerType(const char *name)
{
	// StrPool_addStr(
	int ret = dbg_type_mapping_length++;
	dbg_type_mapping = (DBG_TypeMapping *)realloc(dbg_type_mapping,
												  sizeof(DBG_TypeMapping) * dbg_type_mapping_length);
	dbg_type_mapping[ret] = DBG_TypeMapping(ret, StrPool_addStr(name)->c_str());
	return ret;
}