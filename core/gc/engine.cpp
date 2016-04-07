#include <time.h>
#include <math.h>
#include "collect.h"
#include "core/context.h"
#include "core/object.h"
#include "core/interface/engine.h"
#include "includes/switches.h"

#define CURRENT_OBJECT_COUNT (object_count)
#define CURRENT_COLLECT_THRESHOLD (collect_threshold)
#define CURRENT_COLLECT_THRESHOLD_UNIT (engine->igc_collect_threshold_unit)

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

	if (!obj)
		return;

	if (IS_IGNORED(obj))
		return;

	// obj->incAge();

	SET_BLACK(obj);

	doMark(engine, obj->getBase());

	if (obj->proto_hash && !obj->proto_hash->isConstant()) {
		doMark(engine, obj->proto_hash->getValue());
		if (obj->proto_hash->getSetter()) {
			doMark(engine, obj->proto_hash->getSetter());
		}
		if (obj->proto_hash->getGetter()) {
			doMark(engine, obj->proto_hash->getGetter());
		}
	}

	for (i = obj->hash_table; i; i = i->next) {
		if (!i->isConstant()) {
			doMark(engine, i->getValue());
			if (i->getSetter())
				doMark(engine, i->getSetter());
			if (i->getGetter())
				doMark(engine, i->getGetter());
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

void IGC_CollectEngine::doCollect(bool delete_all)
{
	IGC_CollectUnit *i, *tmp,
					*last = NULL,
					*head = NULL;

	for (i = head = object_chain; i;) {
		tmp = i;
		i = i->next;
		if (delete_all || IS_DISPOSABLE(tmp->obj)) {
			// if (tmp == object_chain) object_chain = i;
			if (tmp == head) {
				head = tmp->next;
			}
			deleteObject(tmp);
		} else {
			last = tmp;
		}
	}
	// for (i = last; i && i->prev; i = i->prev) ;
	object_chain = head;
	object_chain_last = last;

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
	if (engine->object_chain_last)
		object_chain_last = engine->object_chain_last;

	object_count += engine->object_count;

	return;
}

void IGC_CollectEngine::collectGarbage(bool delete_all)
{
	Ink_PardonList::iterator pardon_iter;
	IGC_GreyList::iterator grey_iter;
	IGC_GreyList grey_list;
	vector<DBG_TypeMapping *>::iterator type_iter;

	if (!delete_all) {
		engine->trace->doSelfMark(engine, doMark);
		for (pardon_iter = engine->igc_pardon_list.begin();
			 pardon_iter != engine->igc_pardon_list.end(); pardon_iter++) {
			doMark(*pardon_iter);
		}

		for (type_iter = engine->dbg_type_mapping.begin();
			 type_iter != engine->dbg_type_mapping.end(); type_iter++) {
			doMark((*type_iter)->proto);
		}

		grey_list = engine->getGreyList();
		for (grey_iter = grey_list.begin();
			 grey_iter != grey_list.end(); grey_iter++) {
			doMark(*grey_iter);
		}

		doMark(engine->getInterruptValue());
		doMark(engine->getGlobalReturnValue());
	}
	doCollect(delete_all);

	// printf("\nreduced: %ld in %ld\n", origin - CURRENT_OBJECT_COUNT, origin);
	// printf("GC time duration: %lf\n", (double)(clock() - st) / CLOCKS_PER_SEC);
	// CURRENT_MARK_PERIOD++;

	// engine->initGCCollect();

	return;
}

void IGC_CollectEngine::preMark(IGC_GreyList::size_type max_mark)
{
	IGC_GreyList::iterator grey_iter;
	IGC_GreyList grey_list;
	IGC_GreyList::size_type i;

	grey_list = engine->getGreyList();
	// printf("grey count: %ld\n", grey_list.size());
	for (i = 0, grey_iter = grey_list.begin();
		 grey_iter != grey_list.end() && i <= max_mark; grey_iter++) {
		doMark(*grey_iter);
	}

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
		// tu *= pow(oc / t, 2);
		t += upper((oc - t) / (double)tu) * tu;

		engine->updateMarkPeriod();
		// printf("t increase to: %ld\n", t);
	} else {
		// tu *= pow(oc / t, 2);
		t -= lower((t - oc) / (double)tu) * tu;
		// t -= tu;
		// printf("t recuce to: %ld\n", t);
	}

#ifndef INK_DEBUG_FLAG
	} else {
		// preMark();
	}
#endif

	return;
}

}
