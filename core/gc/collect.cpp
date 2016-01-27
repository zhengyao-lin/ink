#include "collect.h"
#include "core/context.h"
#include "core/object.h"
#include "core/interface/engine.h"

namespace ink {

void IGC_addObject(Ink_InterpreteEngine *current_engine, Ink_Object *obj)
{
	IGC_CollectUnit *new_unit;

	if (current_engine) {
		new_unit = new IGC_CollectUnit(obj);
		current_engine->getCurrentGC()->addUnit(new_unit);
	}

	return;
}

}
