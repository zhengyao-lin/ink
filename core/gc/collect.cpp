#include "collect.h"
#include "core/context.h"
#include "core/object.h"

static long igc_object_count = 0;
static long igc_collect_treshold = IGC_COLLECT_TRESHOLD;
static IGC_CollectUnit *igc_object_chain;
static Ink_ContextChain *igc_global_context = NULL;

void IGC_initGC(Ink_ContextChain *context)
{
	igc_global_context = context->getGlobal();
	return;
}

void cleanMark();
void IGC_addObject(Ink_Object *obj)
{
	IGC_CollectUnit *new_unit;
	IGC_CollectUnit *i;

	if (igc_object_count >= igc_collect_treshold) {
		IGC_collectGarbage(igc_global_context, false);
		if (igc_object_count >= igc_collect_treshold) {
			igc_collect_treshold += igc_object_count;
		}
	}

	new_unit = new IGC_CollectUnit(obj);
	if (igc_object_chain) {
		for (i = igc_object_chain; i && i->next; i = i->next) ;
		i->next = new_unit;
		new_unit->prev = i;
	} else {
		igc_object_chain = new_unit;
	}

	igc_object_count++;

	return;
}

void deleteObject(IGC_CollectUnit *unit)
{
	// printf("delete: object 0x%x\n", unit->obj);
	igc_object_count--;
	delete unit;
	return;
}

void cleanMark()
{
	IGC_CollectUnit *i;

	for (i = igc_object_chain; i; i = i->next) {
		// printf("clean mark: object 0x%x\n", i->obj);
		i->obj->marked = false;
	}

	return;
}

void doMark(Ink_Object *obj)
{
	Ink_HashTable *i;

	if (!obj->marked) obj->marked = true;
	else return;

	for (i = obj->hash_table; i; i = i->next) {
		doMark(i->value);
	}

	return;
}

void doCollect()
{
	IGC_CollectUnit *i, *tmp;

	for (i = igc_object_chain; i;) {
		tmp = i;
		i = i->next;
		if (!tmp->obj->marked) deleteObject(tmp);
	}

	return;
}

void IGC_collectGarbage(Ink_ContextChain *context, bool delete_all, bool if_clean_mark)
{
	Ink_ContextChain *global = context->getGlobal();
	Ink_ContextChain *i;
	Ink_HashTable *j;

	// printf("==========GC START=========\n");

	if (if_clean_mark)
		cleanMark();
	if (!delete_all) {
		for (i = global; i; i = i->inner) {
			doMark(i->context);
		}
	}
	doCollect();

	// printf("===========GC END==========\n");

	return;
}