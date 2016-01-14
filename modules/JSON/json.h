#ifndef _NATIVE_JSON_H_
#define _NATIVE_JSON_H_

#include "core/object.h"

Ink_Object *InkNative_JSON_Encode(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p);
Ink_Object *InkNative_JSON_Decode(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p);

#endif
