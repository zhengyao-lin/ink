#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "object.h"

class Ink_ContextChain {
public:
	Ink_ContextChain *outer;
	Ink_ContextObject *context;
	Ink_ContextChain *inner;

	Ink_ContextChain()
	{
		context = new Ink_ContextObject();
		outer = NULL;
		inner = NULL;
	}

	Ink_ContextChain(Ink_ContextObject *context)
	: context(context)
	{
		outer = NULL;
		inner = NULL;
	}

	void addContext(Ink_ContextObject *c);
	void removeLast();
	Ink_ContextChain *getGlobal();
	Ink_ContextChain *getLocal();
	Ink_Object *searchSlot(const char *slot_id); // from local
	Ink_HashTable *searchSlotMapping(const char *slot_id); // from local

	~Ink_ContextChain()
	{ }
};

#endif