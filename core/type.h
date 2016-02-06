#ifndef _TYPE_H_
#define _TYPE_H_

#include "inttype.h"

namespace ink {

typedef Ink_UInt64 Ink_TypeTag;

#define INK_NULL INK_NULL_tag
#define INK_UNDEFINED INK_UNDEFINED_tag
#define INK_OBJECT INK_OBJECT_tag
#define INK_NUMERIC INK_NUMERIC_tag
#define INK_STRING INK_STRING_tag
#define INK_CONTEXT INK_CONTEXT_tag
#define INK_FUNCTION INK_FUNCTION_tag
#define INK_ARRAY INK_ARRAY_tag
#define INK_UNKNOWN INK_UNKNOWN_tag
#define INK_LAST INK_LAST_tag

enum Ink_TypeTag_tag {
	INK_NULL_tag = 0,
	INK_UNDEFINED_tag,

	INK_OBJECT_tag,
	INK_NUMERIC_tag,
	INK_STRING_tag,
	INK_CONTEXT_tag,
	INK_FUNCTION_tag,
	INK_ARRAY_tag,
	INK_UNKNOWN_tag,
	INK_LAST_tag
};

}

#endif
