#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "object.h"
#include "general.h"

namespace ink {

class Ink_ContextChain {
public:
	Ink_ContextChain *outer;
	Ink_ContextObject *context;
	Ink_ContextChain *inner;

	const char *debug_file_name;
	Ink_LineNoType debug_lineno;
	Ink_Object *debug_creater;

	Ink_ContextChain(Ink_ContextObject *context)
	: context(context)
	{
		outer = NULL;
		inner = NULL;
		debug_file_name = NULL;
		debug_lineno = -1;
		debug_creater = NULL;
	}

	inline void setDebug(const char *file_name, Ink_LineNoType lineno, Ink_Object *creater)
	{
		debug_file_name = file_name;
		debug_lineno = lineno;
		debug_creater = creater;
		return;
	}

	inline const char *getFileName()
	{ return debug_file_name ? debug_file_name : "<unknown input>"; }
	inline Ink_LineNoType getLineno()
	{ return debug_lineno; }
	inline Ink_Object *getCreater()
	{ return debug_creater; }

	Ink_ContextChain *addContext(Ink_ContextObject *c);
	void removeLast(bool if_delete = true);
	Ink_ContextChain *getGlobal();
	Ink_ContextChain *getLocal();
	Ink_Object *searchSlot(Ink_InterpreteEngine *engine, const char *slot_id); // from local
	Ink_HashTable *searchSlotMapping(Ink_InterpreteEngine *engine, const char *slot_id,
									 Ink_ContextChain **found_in = NULL); // from local
	Ink_ContextChain *copyContextChain();
	Ink_ContextChain *copyDeepContextChain(Ink_InterpreteEngine *engine);

	static void disposeContextChain(Ink_ContextChain *head);

	~Ink_ContextChain()
	{ }
};

}

#endif
