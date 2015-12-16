#ifndef _DEBUG_H
#define _DEBUG_H

#include "type.h"

typedef struct DBG_TypeMapping_tag {
	Ink_TypeTag type;
	const char *friendly_name;
} DBG_TypeMapping;

extern DBG_TypeMapping dbg_type_mapping[];

inline const char *getTypeName(Ink_TypeTag type_tag)
{
	return dbg_type_mapping[type_tag].friendly_name;
}

#endif