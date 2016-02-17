#ifndef _COLLECT_H_
#define _COLLECT_H_

#include "../object.h"
#include "../general.h"

#define IGC_COLLECT_TRESHOLD (512)

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
	IGC_MarkType mark;
	Ink_TypeTag type;

	IGC_CollectUnit *object_chain;
	IGC_CollectUnit *object_chain_last;
	Ink_InterpreteEngine *engine;


	IGC_CollectEngine(Ink_InterpreteEngine *engine)
	: engine(engine)
	{
		mark = 0;
		type = INK_NULL;
		object_chain = NULL;
		object_chain_last = NULL;
	}

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

	void doCollect();
	void collectGarbage(bool delete_all = false);
	void checkGC();
	inline void link(IGC_CollectEngine *engine)
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
	}

	~IGC_CollectEngine()
	{ }
};

void IGC_checkGC();
void IGC_addObject(Ink_InterpreteEngine *current_engine, Ink_Object *obj);

}

#endif
