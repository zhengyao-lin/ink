#ifndef _COLLECT_H_
#define _COLLECT_H_

#include "../object.h"

#define IGC_COLLECT_TRESHOLD (512)

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
	//long object_count;
	//long collect_treshold;
	int mark;
	Ink_TypeTag type;
	// IGC_CollectUnit *pardon_chain;
	IGC_CollectUnit *object_chain;
	IGC_CollectUnit *object_chain_last;
	Ink_ContextChain *local_context;

	// IGC_CollectEngine *outer_engine;

	IGC_CollectEngine()
	{
		mark = 0;
		type = INK_NULL;
		// pardon_chain = NULL;
		object_chain = NULL;
		object_chain_last = NULL;
		local_context = NULL;
		// outer_engine = NULL;
	}

	void initContext(Ink_ContextChain *context)
	{
		local_context = context;
	}
	void addUnit(IGC_CollectUnit *unit);
	// void addPardon(Ink_Object *obj);
	void cleanMark();
	static void doMark(Ink_Object *obj);
	static void deleteObject(IGC_CollectUnit *unit);
	static void disposeChainWithoutDelete(IGC_CollectUnit *chain);

	void doCollect();
	void collectGarbage(bool delete_all = false);
	void checkGC();
	void link(IGC_CollectEngine *engine);
	// void realloc(Ink_Object *obj); // move from the origin gc engine to self
	// bool remove(Ink_Object *obj);

	~IGC_CollectEngine()
	{
		// collectGarbage(true);
		//if (pardon_chain)
		//	disposeChainWithoutDelete(pardon_chain);
	}
};

void IGC_initGC(IGC_CollectEngine *engine);
void IGC_checkGC();
void IGC_addObject(Ink_Object *obj);
void IGC_collectGarbage(Ink_ContextChain *context, bool delete_all = false, bool if_clean_mark = true);

#endif