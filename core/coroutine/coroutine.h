#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <stack>
#include <ucontext.h>
#include "../object.h"
#include "../context.h"
#include "../gc/collect.h"

class Ink_UContext: public Ink_Object {
public:
	ucontext_t context;

	Ink_UContext(ucontext_t context)
	: context(context)
	{ type = INK_NULL; }
};

class Ink_GCEngine: public Ink_Object {
public:
	IGC_CollectEngine *gc_engine;

	Ink_GCEngine(IGC_CollectEngine *gc_engine)
	: gc_engine(gc_engine)
	{ type = INK_NULL; }

	virtual ~Ink_GCEngine()
	{
		delete gc_engine;
	}
};

#endif