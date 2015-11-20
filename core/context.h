#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "object.h"

class Ink_ContextChain {
public:
	Ink_ContextChain *outer = NULL;
	Ink_ContextObject *context;
	Ink_ContextChain *inner = NULL;

	// Compiling Global Context
	bool flag_is_fnuction_call = false;
	Ink_ContextChain *tmp_context_head = NULL;

	Ink_ContextChain()
	{
		context = new Ink_ContextObject();
	}

	Ink_ContextChain(Ink_ContextObject *context)
	: context(context)
	{ }

	Ink_ContextChain(Ink_HashTable *hash_table)
	{
		context = new Ink_ContextObject(hash_table);
	}

	Ink_ContextChain *addContext(Ink_ContextObject *c);
	void removeLast();
	Ink_ContextChain *getGlobal();
	Ink_ContextChain *getLocal();
	Ink_Object *searchSlot(const char *slot_id); // from local
	Ink_HashTable *searchSlotMapping(const char *slot_id); // from local

	// CGC setters
	void setThisContext(Ink_Object *base_obj)
	{
		Ink_ContextChain *new_context = getLocal()->addContext(new Ink_ContextObject(base_obj->hash_table));
		if (!tmp_context_head)
			tmp_context_head = new_context;
		return;
	}

	void resetThisContext()
	{
		if (tmp_context_head)
			disposeContextChain(tmp_context_head);
		tmp_context_head = NULL;
		return;
	}

	static void disposeContextChain(Ink_ContextChain *head);

	~Ink_ContextChain()
	{ }
};

#endif