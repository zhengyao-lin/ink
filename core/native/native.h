#ifndef _NATIVE_H_
#define _NATIVE_H_

#include "../object.h"
#include "../context.h"
#include "general.h"

typedef struct InkNative_MethodTable_tag {
	const char *name;
	Ink_Object *func;
} InkNative_MethodTable;

bool isTrue(Ink_Object *cond);
bool isEqual(Ink_Object *a, Ink_Object *b);

Ink_Object *InkNative_Array_Index(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Array_Push(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Array_Size(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Array_Each(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Array_Remove(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Array_Rebuild(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);

Ink_Object *InkNative_Function_Insert(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Function_RangeCall(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Function_GetExp(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Function_GetScope(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);

Ink_Object *InkNative_Numeric_Add(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_Sub(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_Mul(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_Div(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_Mod(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_Equal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_NotEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_Greater(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_Less(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_GreaterOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_LessOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_Add_Unary(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_Sub_Unary(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Numeric_ToString(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);

Ink_Object *InkNative_Object_Bond(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_Debond(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_Not(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_Equal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_NotEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_Index(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_New(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_Delete(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_Clone(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_SetGetter(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_SetSetter(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_Object_Each(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);

Ink_Object *InkNative_String_Add(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_String_Index(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_String_Length(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_String_Greater(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_String_Less(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_String_GreaterOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);
Ink_Object *InkNative_String_LessOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p = NULL);

Ink_Object **arrayValueToObject(Ink_ArrayValue val);

#endif
