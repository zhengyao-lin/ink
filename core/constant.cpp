#include <assert.h>
#include "constant.h"
#include "general.h"
#include "object.h"

namespace ink {

Ink_Object *Ink_NumericConstant::toObject(Ink_InterpreteEngine *engine)
{
	return new Ink_Numeric(engine, value);
}

Ink_Object *Ink_StringConstant::toObject(Ink_InterpreteEngine *engine)
{
	return new Ink_String(engine, *value);
}

}
