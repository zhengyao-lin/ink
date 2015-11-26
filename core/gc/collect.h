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
		delete obj;
	}
};

void IGC_initGC(Ink_ContextChain *context);
void IGC_checkGC();
void IGC_addObject(Ink_Object *obj);
void IGC_collectGarbage(Ink_ContextChain *context, bool delete_all = false, bool if_clean_mark = true);

#endif