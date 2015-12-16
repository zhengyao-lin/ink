#ifndef _CORE_GENERAL_H_
#define _CORE_GENERAL_H_

#include "error.h"

template <typename T1, typename T2, typename T3>
class triple {
public:
	T1 first;
	T2 second;
	T3 third;

	triple(T1 first)
	: first(first)
	{ }

	triple(T1 first, T2 second)
	: first(first), second(second)
	{ }

	triple(T1 first, T2 second, T3 third)
	: first(first), second(second), third(third)
	{ }
};

// inline functions
/*
inline Ink_Object *getSlotWithProto(Ink_Object *base, const char *name)
{
	return Ink_HashExpression::getSlot(base, name);
}
inline Ink_Object *callMethod(Ink_Object *base, const char *method_name,
							  unsigned int argc = 0, Ink_Object **argv = NULL, Ink_Object *this_p = NULL)
{
	if (!(base = getSlotWithProto(base, method_name))) {
		InkWarn_Failed_Finding_Slot(const char *name)
		return new Ink_NullObject();
	}
}*/

#endif