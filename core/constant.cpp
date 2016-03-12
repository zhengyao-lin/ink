#include <assert.h>
#include "constant.h"
#include "general.h"
#include "object.h"

namespace ink {

Ink_Object *Ink_Constant::toObject(Ink_InterpreteEngine *engine)
{
	switch (type) {
		case INK_STRING:
			return new Ink_String(engine, *str);
		case INK_NUMERIC:
			return new Ink_Numeric(engine, num);
		default:
			// shouldn't be here
			assert(0);
	}

	return NULL;
}

Ink_Constant *Ink_Constant::fromObject(Ink_Object *obj)
{
	if (!obj) return NULL;

	switch (obj->type) {
		case INK_STRING:
			return new Ink_Constant(as<Ink_String>(obj)->getWValue());
		case INK_NUMERIC:
			return new Ink_Constant(as<Ink_Numeric>(obj)->value);
		default:;
	}

	return NULL;
}

}
