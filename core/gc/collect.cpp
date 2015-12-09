#include "collect.h"
#include "core/context.h"
#include "core/object.h"
#include "../../interface/engine.h"

extern Ink_InterpreteEngine *current_interprete_engine;

void IGC_initGC(IGC_CollectEngine *engine)
{
	current_interprete_engine->setCurrentGC(engine);
	return;
}

void IGC_addObject(Ink_Object *obj)
{
	IGC_CollectUnit *new_unit;

	if (current_interprete_engine) {
		new_unit = new IGC_CollectUnit(obj);
		current_interprete_engine->getCurrentGC()->addUnit(new_unit);
	}

	return;
}