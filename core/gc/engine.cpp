#include <time.h>
#include "collect.h"
#include "core/context.h"
#include "core/object.h"
#include "core/interface/engine.h"
#include "includes/switches.h"

#define CURRENT_OBJECT_COUNT (object_count)
#define CURRENT_COLLECT_THRESHOLD (collect_threshold)
#define CURRENT_COLLECT_THRESHOLD_UNIT (engine->igc_collect_threshold_unit)
#define CURRENT_MARK_PERIOD (engine->igc_mark_period)

namespace ink {

IGC_CollectEngine::IGC_CollectEngine(Ink_InterpreteEngine *engine)
: engine(engine)
{
	type = INK_NULL;
	object_chain = NULL;
	object_chain_last = NULL;
	object_count = 0;
	collect_threshold = engine->igc_collect_threshold;
}

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
	else return;

	doMark(engine, obj->getBase());

	if (obj->proto_hash) {
		doMark(engine, obj->proto_hash->getValue());
		if (obj->proto_hash->setter) {
			doMark(engine, obj->proto_hash->setter);
		}
		if (obj->proto_hash->getter) {
			doMark(engine, obj->proto_hash->getter);
		}
		if (obj->proto_hash->bonding) {
			engine->addGCBonding(obj->proto_hash, obj->proto_hash->bonding);
		}
	}

	for (i = obj->hash_table; i; i = i->next) {
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
	Ink_PardonList::iterator pardon_iter;
	vector<DBG_TypeMapping *>::iterator type_iter;
	//clock_t st;

	//st = clock();
	if (!delete_all) {
		for (i = engine->trace->getGlobal();
			 i; i = i->inner) {
			doMark(i->context);
		}
		for (pardon_iter = engine->igc_pardon_list.begin();
			 pardon_iter != engine->igc_pardon_list.end(); pardon_iter++) {
			doMark(*pardon_iter);
		}
		for (type_iter = engine->dbg_type_mapping.begin();
			 type_iter != engine->dbg_type_mapping.end(); type_iter++) {
			doMark((*type_iter)->proto);
		}
		doMark(engine->getInterruptValue());

	}
	doCollect();
	// printf("\nreduced: %ld in %ld\n", origin - CURRENT_OBJECT_COUNT, origin);
	// printf("GC time duration: %lf\n", (double)(clock() - st) / CLOCKS_PER_SEC);
	CURRENT_MARK_PERIOD++;

	engine->initGCCollect();

	return;
}

#define upper(i) ((i) > (IGC_ObjectCountType)(i) ? (IGC_ObjectCountType)(i) + 1 : (IGC_ObjectCountType)(i))
#define lower(i) ((IGC_ObjectCountType)(i))
#define oc (CURRENT_OBJECT_COUNT)
#define t (CURRENT_COLLECT_THRESHOLD)
#define tu (CURRENT_COLLECT_THRESHOLD_UNIT)

void IGC_CollectEngine::checkGC()
{
#ifndef INK_DEBUG_FLAG
	if (oc >= t) {
#endif

	// printf("before: oc: %ld; ", oc);

	collectGarbage();

	// printf("after collect: oc: %ld, t: %ld; ", oc, t);
	if (oc >= t) { /* increase */
		t += upper((oc - t) / (double)tu) * tu;
		// printf("t increase to: %ld\n", t);
	} else { /* reduce */
		t -= lower((t - oc) / (double)tu) * tu;
		// printf("t recuce to: %ld\n", t);
	}

#ifndef INK_DEBUG_FLAG
	}
#endif
	return;
}

}
