#include <time.h>
#include "collect.h"
#include "core/context.h"
#include "core/object.h"
#include "core/interface/engine.h"
#include "includes/switches.h"

#define CURRENT_OBJECT_COUNT (engine->igc_object_count)
#define CURRENT_COLLECT_TRESHOLD (engine->igc_collect_treshold)
#define CURRENT_MARK_PERIOD (engine->igc_mark_period)

namespace ink {

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

void IGC_CollectEngine::doMark(Ink_InterpreteEngine *engine, Ink_Object *obj)
{
	Ink_HashTable *i;

	if (obj && obj->mark != CURRENT_MARK_PERIOD) obj->mark = CURRENT_MARK_PERIOD;
	else {
		return;
	}

	for (i = obj->hash_table; i; i = i->next) {
		if (i->getValue())
			doMark(engine, i->getValue());
		if (i->setter)
			doMark(engine, i->setter);
		if (i->getter)
			doMark(engine, i->getter);
		if (i->bonding) {
			engine->addGCBonding(i, i->bonding);
		}
	}

	obj->doSelfMark(engine, doMark);

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
	//clock_t st;

	//st = clock();
	engine->initGCCollect();
	if (!delete_all) {
		for (i = engine->trace->getGlobal();
			 i; i = i->inner) {
			doMark(i->context);
		}
		doMark(engine->CGC_interrupt_value);
	}
	doCollect();
	//printf("GC time duration: %lf\n", (double)(clock() - st) / CLOCKS_PER_SEC);
	CURRENT_MARK_PERIOD++;

	return;
}

void IGC_CollectEngine::checkGC()
{
#ifdef INK_DEBUG_FLAG
	collectGarbage();
	if (CURRENT_OBJECT_COUNT >= CURRENT_COLLECT_TRESHOLD) {
		CURRENT_COLLECT_TRESHOLD += CURRENT_OBJECT_COUNT;
	}
#else
	if (CURRENT_OBJECT_COUNT >= CURRENT_COLLECT_TRESHOLD) {
		collectGarbage();
		if (CURRENT_OBJECT_COUNT >= CURRENT_COLLECT_TRESHOLD) {
			CURRENT_COLLECT_TRESHOLD += CURRENT_OBJECT_COUNT;
		}
	}
#endif
	return;
}

}
