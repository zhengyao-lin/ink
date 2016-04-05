#ifndef _COLLECT_H_
#define _COLLECT_H_

#include "../object.h"
#include "../general.h"

#define IGC_COLLECT_THRESHOLD_UNIT (128)

namespace ink {

class IGC_CollectUnit {
public:
	IGC_CollectUnit *prev;
	Ink_Object *obj;
	IGC_CollectUnit *next;

	IGC_CollectUnit(Ink_Object *obj)
	: obj(obj)
	{
		prev = NULL;
		next = NULL;
	}

	~IGC_CollectUnit()
	{
		if (prev)
			prev->next = next;
		if (next)
			next->prev = prev;
		if (obj)
			delete obj;
	}
};

class IGC_CollectEngine {
public:
	Ink_TypeTag type;

	IGC_CollectUnit *object_chain;
	IGC_CollectUnit *object_chain_last;
	Ink_InterpreteEngine *engine;

	IGC_ObjectCountType object_count;
	IGC_ObjectCountType collect_threshold;

	IGC_CollectEngine(Ink_InterpreteEngine *engine);

	void addUnit(IGC_CollectUnit *unit);
	// void addPardon(Ink_Object *obj);
	inline void doMark(Ink_Object *obj)
	{
		doMark(engine, obj);
		return;
	}
	static void doMark(Ink_InterpreteEngine *engine, Ink_Object *obj);
	void deleteObject(IGC_CollectUnit *unit);
	void disposeChainWithoutDelete(IGC_CollectUnit *chain);

	void doCollect(bool delete_all = false);
	void collectGarbage(bool delete_all = false);
	void checkGC();
	void link(IGC_CollectEngine *engine);

	~IGC_CollectEngine()
	{ }
};

void IGC_checkGC();
void IGC_addObject(Ink_InterpreteEngine *current_engine, Ink_Object *obj);
void IGC_addObject(IGC_CollectEngine *engine, Ink_Object *obj);

}

#endif
