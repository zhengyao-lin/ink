#include <sstream>
#include "../object.h"
#include "../context.h"
#include "native.h"

Ink_Object *InkNative_String_Add(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_String *tmp;

	ASSUME_BASE_TYPE(engine, INK_STRING);

	if (!checkArgument(engine, argc, 1)) {
		return NULL_OBJ;
	}

	if ((tmp = getStringVal(engine, context, argv[0])) != NULL) {
		return new Ink_String(engine, as<Ink_String>(base)->value + tmp->value);
	}

	InkWarn_Invalid_Argument_For_String_Add(engine, argv[0]->type);
	return NULL_OBJ;
}

Ink_Object *InkNative_String_Index(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	int index;

	ASSUME_BASE_TYPE(engine, INK_STRING);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		InkWarn_Method_Fallthrough(engine, INK_OBJECT);
		return InkNative_Object_Index(engine, context, argc, argv, this_p);
	}
	index = as<Ink_Numeric>(argv[0])->value;

	return new Ink_String(engine, as<Ink_String>(base)->value.substr(index, 1));
}

Ink_Object *InkNative_String_Length(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);

	return new Ink_Numeric(engine, as<Ink_String>(base)->value.length());
}

Ink_Object *InkNative_String_Greater(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->value.c_str()[0] > as<Ink_String>(argv[0])->value.c_str()[0]);
}

Ink_Object *InkNative_String_Less(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->value.c_str()[0] < as<Ink_String>(argv[0])->value.c_str()[0]);
}

Ink_Object *InkNative_String_GreaterOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->value.c_str()[0] >= as<Ink_String>(argv[0])->value.c_str()[0]);
}

Ink_Object *InkNative_String_LessOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->value.c_str()[0] <= as<Ink_String>(argv[0])->value.c_str()[0]);
}

extern int string_native_method_table_count;
extern InkNative_MethodTable string_native_method_table[];

void Ink_String::Ink_StringMethodInit()
{
	InkNative_MethodTable *table = string_native_method_table;
	int i, count = string_native_method_table_count;

	for (i = 0; i < count; i++) {
		setSlot(table[i].name, table[i].func);
	}
}
