#include "debug.h"
#include "object.h"

DBG_TypeMapping dbg_type_mapping[] = 
{
	{ INK_NULL,			"null" },
	{ INK_UNDEFINED,	"undefined" },
	{ INK_OBJECT,		"object" },
	{ INK_NUMERIC,		"numeric" },
	{ INK_STRING,		"string" },
	{ INK_CONTEXT,		"context" },
	{ INK_FUNCTION,		"function" },
	{ INK_ARRAY,		"array" },
	{ INK_UNKNOWN,		"unknown" },
	{ INK_CUSTOM,		"custom" }
};