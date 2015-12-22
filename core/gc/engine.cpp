#include "collect.h"
#include "core/context.h"
#include "core/object.h"
#include "interface/engine.h"

#define CURRENT_OBJECT_COUNT (Ink_getCurrentEngine()->igc_object_count)
#define CURRENT_COLLECT_TRESHOLD (Ink_getCurrentEngine()->igc_collect_treshold)
#define CURRENT_MARK_PERIOD (Ink_getCurrentEngine()->igc_mark_period)

void IGC_CollectEngine::addUnit(IGC_CollectUnit *unit)
{
	if (object_chain_last) {
		object_chain_last->next = unit;
		unit->prev = object_chain_last;
	} else {
		object_chain = unit;
	}
	object_chain_last = unit;
	CURRENT_OBJECT_COUNT++;

	return;
}

void IGC_CollectEngine::doMark(Ink_Object *obj)
{
	Ink_HashTable *i;

	if (obj->mark != CURRENT_MARK_PERIOD) obj->mark = CURRENT_MARK_PERIOD;
	else {
		return;
	}

	for (i = obj->hash_table; i; i = i->next) {
		if (i->getValue())
			doMark(i->getValue());
		if (i->setter)
			doMark(i->setter);
		if (i->getter)
			doMark(i->getter);
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
				doMark(arr->value[i]->getValue());
		}
	}

	return;
}

void IGC_CollectEngine::deleteObject(IGC_CollectUnit *unit)
{
	CURRENT_OBJECT_COUNT--;
	delete unit;
	return;
}

void IGC_CollectEngine::disposeChainWithoutDelete(IGC_CollectUnit *chain)
{
	IGC_CollectUnit *i, *tmp;
	for (i = chain; i;) {
		tmp = i;
		i = i->next;
		tmp->obj = NULL;
		tmp->prev = NULL;
		tmp->next = NULL;
		delete tmp;
	}

	return;
}

void IGC_CollectEngine::doCollect()
{
	IGC_CollectUnit *i, *tmp, *last = NULL;

	for (i = object_chain; i;) {
		tmp = i;
		i = i->next;
		if (tmp->obj->mark != CURRENT_MARK_PERIOD) {
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

void IGC_CollectEngine::collectGarbage(bool delete_all)
{
	Ink_ContextChain *i;

	if (!delete_all) {
		for (i = Ink_getCurrentEngine()->trace->getGlobal();
			 i; i = i->inner) {
			doMark(i->context);
		}
	}
	doCollect();
	CURRENT_MARK_PERIOD++;

	return;
}

void IGC_CollectEngine::checkGC()
{
	if (CURRENT_OBJECT_COUNT >= CURRENT_COLLECT_TRESHOLD) {
		collectGarbage();
		if (CURRENT_OBJECT_COUNT >= CURRENT_COLLECT_TRESHOLD) {
			CURRENT_COLLECT_TRESHOLD += CURRENT_OBJECT_COUNT;
		}
	}
	return;
}