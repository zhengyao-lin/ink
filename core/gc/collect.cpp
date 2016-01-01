#include "collect.h"
#include "core/context.h"
#include "core/object.h"
#include "core/interface/engine.h"

void IGC_initGC(IGC_CollectEngine *engine)
{
	Ink_getCurrentEngine()->setCurrentGC(engine);
	return;
}

void IGC_addObject(Ink_Object *obj)
{
	IGC_CollectUnit *new_unit;
	Ink_InterpreteEngine *current_engine = Ink_getCurrentEngine();

	if (current_engine) {
		new_unit = new IGC_CollectUnit(obj);
		current_engine->getCurrentGC()->addUnit(new_unit);
	}

	return;
}