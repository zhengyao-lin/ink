#include "collect.h"
#include "core/context.h"
#include "core/object.h"

static IGC_CollectUnit *igc_object_chain;

void IGC_addObject(Ink_Object *obj)
{
	IGC_CollectUnit *new_unit = new IGC_CollectUnit(obj);
	IGC_CollectUnit *i;

	if (igc_object_chain) {
		for (i = igc_object_chain; i && i->next; i = i->next) ;
		i->next = new_unit;
		new_unit->prev = i;
	} else {
		igc_object_chain = new_unit;
	}

	return;
}

void deleteObject(IGC_CollectUnit *unit)
{
	printf("delete: object 0x%x\n", unit->obj);
	if (unit->prev)
		unit->prev->next = unit->next;
	if (unit->next)
		unit->next->prev = unit->prev;
	delete unit;
	return;
}

void cleanMark()
{
	IGC_CollectUnit *i;

	for (i = igc_object_chain; i; i = i->next) {
		printf("clean mark: object 0x%x\n", i->obj);
		i->obj->marked = false;
	}

	return;
}

void doMark(Ink_Object *obj)
{
	Ink_HashTable *i;

	for (i = obj->hash_table; i; i = i->next) {
		doMark(i->value);
	}
	obj->marked = true;

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

void IGC_collectGarbage(Ink_ContextChain *context, bool delete_all = false)
{
	Ink_ContextChain *global = context->getGlobal();
	Ink_ContextChain *i;
	Ink_HashTable *j;

	printf("==========GC START=========\n");

	cleanMark();
	if (!delete_all) {
		for (i = global; i; i = i->inner) {
			for (j = i->context->hash_table; j; j = j->next) {
				doMark(j->value);
			}
		}
	}
	doCollect();

	printf("==========GC END=========\n");

	return;
}