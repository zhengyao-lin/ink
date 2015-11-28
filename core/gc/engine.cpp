#include "collect.h"
#include "core/context.h"
#include "core/object.h"

extern long igc_object_count;
extern long igc_collect_treshold;
extern int igc_mark_period;

void IGC_CollectEngine::addUnit(IGC_CollectUnit *unit)
{
	if (object_chain_last) {
		object_chain_last->next = unit;
		unit->prev = object_chain_last;
	} else {
		object_chain = unit;
	}
	object_chain_last = unit;
	igc_object_count++;

	return;
}

void IGC_CollectEngine::cleanMark()
{
	IGC_CollectUnit *i;

	for (i = object_chain; i; i = i->next) {
		i->obj->marked = false;
	}

	return;
}

void IGC_CollectEngine::doMark(Ink_Object *obj)
{
	Ink_HashTable *i;

	if (obj->marked != igc_mark_period) obj->marked = igc_mark_period;
	else {
		//printf("========skip!\n");
		return;
	}

	for (i = obj->hash_table; i; i = i->next) {
		//printf("%s\n", i->key);
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

void IGC_CollectEngine::deleteObject(IGC_CollectUnit *unit)
{
	igc_object_count--;
	delete unit;
	return;
}

void IGC_CollectEngine::doCollect()
{
	IGC_CollectUnit *i, *tmp, *last = NULL;

	for (i = object_chain; i;) {
		tmp = i;
		i = i->next;
		if (tmp->obj->marked != igc_mark_period) {
			// if (tmp == object_chain) object_chain = i;
			deleteObject(tmp);
		} else {
			last = tmp;
		}
	}
	for (i = last; i && i->prev; i = i->prev) ;
	object_chain = i;
	object_chain_last = last;

	return;
}

void IGC_CollectEngine::collectGarbage(bool delete_all, bool if_clean_mark)
{
	Ink_ContextChain *global = local_context->getGlobal();
	Ink_ContextChain *i;

	printf("========== GC INTERRUPT =========\n");

	//if (if_clean_mark)
	//	cleanMark();
	if (!delete_all) {
		for (i = global; i; i = i->inner) {
			// printf("hi\n");
			doMark(i->context);
		}
	}
	doCollect();
	igc_mark_period++;

	return;
}

void IGC_CollectEngine::checkGC()
{
	if (igc_object_count >= igc_collect_treshold) {
		collectGarbage();
		if (igc_object_count >= igc_collect_treshold) {
			igc_collect_treshold += igc_object_count;
		}
	}
	return;
}

void IGC_CollectEngine::link(IGC_CollectEngine *engine)
{
	if (object_chain_last) {
		object_chain_last->next = engine->object_chain;
		if (engine->object_chain)
			engine->object_chain->prev = object_chain_last;
	} else {
		object_chain = engine->object_chain;
	}
	object_chain_last = engine->object_chain_last;
}