#include "hash.h"
#include "object.h"
#include "general.h"
#include "interface/engine.h"

namespace ink {

void Ink_Array::disposeArrayValue()
{
	Ink_Object *obj;
	Ink_ArrayValue::size_type i;
	for (i = 0; i < value.size(); i++) {
		if (value[i]) {
			if ((obj = engine->getGlobalReturnValue()) != NULL
				&& obj->address == value[i]) {
				obj->address = NULL;
			}
			engine->breakUnreachableBonding(value[i]);
			delete value[i];
		}
	}
	return;
}

void Ink_Array::doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker)
{
	Ink_ArrayValue::size_type i;
	for (i = 0; i < value.size(); i++) {
		if (value[i]) {
			marker(engine, value[i]->getValue());
			if (value[i]->bonding) {
				engine->addGCBonding(value[i], value[i]->bonding);
			}
		}
	}
	return;
}

}