#include "collect.h"
#include "core/context.h"
#include "core/object.h"

long igc_object_count = 0;
long igc_collect_treshold = IGC_COLLECT_TRESHOLD;
IGC_CollectEngine *global_engine = NULL;
IGC_CollectEngine *current_engine = NULL;
int igc_mark_period = 1;

void IGC_initGC(IGC_CollectEngine *engine, bool is_global)
{
	current_engine = engine;
	if (is_global) global_engine = engine;
	return;
}

void IGC_addUnit(IGC_CollectUnit *unit)
{
	if (current_engine)
		current_engine->addUnit(unit);

	return;
}


void IGC_addObject(Ink_Object *obj)
{
	IGC_CollectUnit *new_unit;

	new_unit = new IGC_CollectUnit(obj);
	IGC_addUnit(new_unit);

	return;
}