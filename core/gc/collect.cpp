#include "collect.h"
#include "core/context.h"
#include "core/object.h"

static long igc_object_count = 0;
static long igc_collect_treshold = IGC_COLLECT_TRESHOLD;
static IGC_CollectUnit *igc_object_chain = NULL;
static IGC_CollectUnit *igc_object_chain_last = NULL;
static Ink_ContextChain *igc_global_context = NULL;

void IGC_initGC(Ink_ContextChain *context)
{
	igc_global_context = context->getGlobal();
	return;
}

void IGC_addUnit(IGC_CollectUnit *unit)
{
	if (igc_object_chain_last) {
		igc_object_chain_last->next = unit;
		unit->prev = igc_object_chain_last;
	} else {
		igc_object_chain = unit;
	}
	igc_object_chain_last = unit;
	igc_object_count++;

	return;
}

void IGC_checkGC()
{
	if (igc_object_count >= igc_collect_treshold) {
		IGC_collectGarbage(igc_global_context, false);
		if (igc_object_count >= igc_collect_treshold) {
			igc_collect_treshold += igc_object_count;
		}
	}
	return;
}

void IGC_addObject(Ink_Object *obj)
{
	IGC_CollectUnit *new_unit;

	new_unit = new IGC_CollectUnit(obj);
	IGC_addUnit(new_unit);

	// IGC_checkGC();

	return;
}

void deleteObject(IGC_CollectUnit *unit)
{
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

	if (obj->type == INK_FUNCTION) {
		Ink_FunctionObject *func = as<Ink_FunctionObject>(obj);
		Ink_ContextChain *global = func->closure_context->getGlobal();
		Ink_ContextChain *j;

		for (j = global; j; j = j->inner) {
			doMark(j->context);
		}
	} else if (obj->type == INK_ARRAY) {
		Ink_Array *arr = as<Ink_Array>(obj);
		unsigned int i;
		for (i = 0; i < arr->value.size(); i++) {
			if (arr->value[i])
				doMark(arr->value[i]->value);
		}
	}

	return;
}

void doCollect()
{
	IGC_CollectUnit *i, *tmp;

	for (i = igc_object_chain; i;) {
		tmp = i;
		i = i->next;
		if (!tmp->obj->marked) {
			if (tmp == igc_object_chain) igc_object_chain = i;
			deleteObject(tmp);
		}
	}
	for (i = igc_object_chain; i && i->next; i = i->next) ;
	igc_object_chain_last = i;

	return;
}

void IGC_collectGarbage(Ink_ContextChain *context, bool delete_all, bool if_clean_mark)
{
	Ink_ContextChain *global = context->getGlobal();
	Ink_ContextChain *i;

	printf("========== GC INTERRUPT =========\n");

	if (if_clean_mark)
		cleanMark();
	if (!delete_all) {
		for (i = global; i; i = i->inner) {
			doMark(i->context);
		}
	}
	doCollect();

	return;
}