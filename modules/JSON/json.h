#ifndef _MOD_JSON_H_
#define _MOD_JSON_H_

#include "core/object.h"
#include "core/error.h"
#include "core/interface/engine.h"

using namespace ink;

Ink_Object *InkNative_JSON_Encode(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);
Ink_Object *InkNative_JSON_Decode(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p);

inline void
InkWarn_JSON_Cyclic_Reference(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, "Cyclic object reference");
	return;
}

#endif
