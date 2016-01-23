#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "object.h"
#include "general.h"

class Ink_ContextChain {
public:
	Ink_ContextChain *outer;
	Ink_ContextObject *context;
	Ink_ContextChain *inner;

	Ink_LineNoType debug_lineno;
	Ink_Object *debug_creater;

	Ink_ContextChain(Ink_ContextObject *context)
	: context(context)
	{
		outer = NULL;
		inner = NULL;
		debug_lineno = -1;
		debug_creater = NULL;
	}

	inline void setDebug(Ink_LineNoType lineno, Ink_Object *creater)
	{
		debug_lineno = lineno;
		debug_creater = creater;
		return;
	}

	inline Ink_LineNoType getLineno()
	{ return debug_lineno; }
	inline Ink_Object *getCreater()
	{ return debug_creater; }

	Ink_ContextChain *addContext(Ink_ContextObject *c);
	void removeLast(bool if_delete = true);
	Ink_ContextChain *getGlobal();
	Ink_ContextChain *getLocal();
	Ink_Object *searchSlot(Ink_InterpreteEngine *engine, const char *slot_id); // from local
	Ink_HashTable *searchSlotMapping(Ink_InterpreteEngine *engine, const char *slot_id); // from local
	Ink_ContextChain *copyContextChain();
	Ink_ContextChain *copyDeepContextChain(Ink_InterpreteEngine *engine);

	static void disposeContextChain(Ink_ContextChain *head);

	~Ink_ContextChain()
	{ }
};

#endif
