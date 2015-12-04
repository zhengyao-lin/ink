#ifndef _NATIVE_H_
#define _NATIVE_H_

#include "../object.h"
#include "../context.h"

typedef struct InkNative_MethodTable_tag {
	const char *name;
	Ink_Object *func;
} InkNative_MethodTable;

bool isTrue(Ink_Object *cond);
bool isEqual(Ink_Object *a, Ink_Object *b);

Ink_Object *InkNative_Array_Index(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Array_Push(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Array_Size(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Array_Remove(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Array_Rebuild(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);

Ink_Object *InkNative_Function_Insert(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Function_RangeCall(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Function_GetExp(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);

Ink_Object *InkNative_Integer_Add(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_Sub(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_Mul(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_Div(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_Equal(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_NotEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_Greater(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_Less(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_GreaterOrEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_LessOrEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_Add_Unary(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Integer_Sub_Unary(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);

Ink_Object *InkNative_Object_Bond(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Object_Debond(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Object_Not(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Object_Equal(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Object_NotEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Object_Index(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Object_New(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_Object_Clone(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);

Ink_Object *InkNative_String_Add(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
Ink_Object *InkNative_String_Index(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);

#endif