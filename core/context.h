#ifndef _CONTEXT_H_
#define _CONTEXT_H_

#include "object.h"
#include "general.h"

namespace ink {

struct Ink_ContextChain_sub {
	Ink_ContextChain_sub *outer;
	Ink_ContextObject *context;
	Ink_ContextChain_sub *inner;

	const char *debug_file_name;
	Ink_LineNoType debug_lineno;
	Ink_Object *debug_creater;

	Ink_ContextChain_sub(Ink_ContextObject *context)
	: context(context)
	{
		outer = NULL;
		inner = NULL;

		debug_file_name = NULL;
		debug_lineno = -1;
		debug_creater = NULL;
	}

	inline Ink_ContextObject *getContext()
	{
		return context;
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
};

class Ink_ContextChain {
public:
	Ink_ContextChain_sub *head;
	Ink_ContextChain_sub *tail;

	Ink_ContextChain()
	{
		tail = head = NULL;
	}

	Ink_ContextChain(Ink_ContextObject *context)
	{
		tail = head = new Ink_ContextChain_sub(context);
	}

	inline Ink_ContextChain_sub *getTail()
	{ return tail; }

	Ink_ContextChain_sub *addContext(Ink_ContextObject *c);
	void removeLast(bool if_delete = true);
	void removeContext(Ink_ContextObject *c, bool if_delete = true);
	Ink_ContextObject *getGlobal();
	Ink_ContextObject *getLocal();
	Ink_Object *searchSlot(Ink_InterpreteEngine *engine, const char *slot_id); // from local
	Ink_HashTable *searchSlotMapping(Ink_InterpreteEngine *engine, const char *slot_id,
									 Ink_ContextObject **found_in = NULL); // from local
	Ink_ContextChain *copyContextChain();
	Ink_ContextChain *copyDeepContextChain(Ink_InterpreteEngine *engine);
	void doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker);

	static void disposeContextChain(Ink_ContextChain *head);

	~Ink_ContextChain()
	{
		Ink_ContextChain_sub *i, *tmp;

		for (i = head; i;) {
			tmp = i;
			i = i->inner;
			delete tmp;
		}
	}
};

}

#endif
