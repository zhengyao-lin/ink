#include "../object.h"
#include "../context.h"
#include "native.h"

class Ink_Numeric;

Ink_Object *InkNative_Numeric_Add(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	if (!checkArgument(argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(as<Ink_Numeric>(base)->value + as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Sub(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	if (!checkArgument(argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(as<Ink_Numeric>(base)->value - as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Mul(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	if (!checkArgument(argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(as<Ink_Numeric>(base)->value * as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Div(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	if (!checkArgument(argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(as<Ink_Numeric>(base)->value / as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Mod(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	if (!checkArgument(argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric((int)as<Ink_Numeric>(base)->value % (int)as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Equal(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_NUMERIC)) {
		return InkNative_Object_Equal(context, argc, argv, this_p);
	}

	return new Ink_Numeric(as<Ink_Numeric>(base)->value == as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_NotEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	if (!argc) return NULL_OBJ;

	if (!checkArgument(false, argc, argv, 1, INK_NUMERIC)) {
		return InkNative_Object_NotEqual(context, argc, argv, this_p);
	}

	return new Ink_Numeric(as<Ink_Numeric>(base)->value != as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Greater(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	if (!checkArgument(argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(as<Ink_Numeric>(base)->value > as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Less(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	if (!checkArgument(argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(as<Ink_Numeric>(base)->value < as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_GreaterOrEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	if (!checkArgument(argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(as<Ink_Numeric>(base)->value >= as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_LessOrEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	if (!checkArgument(argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(as<Ink_Numeric>(base)->value <= as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Add_Unary(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	return new Ink_Numeric(as<Ink_Numeric>(base)->value);
}

Ink_Object *InkNative_Numeric_Sub_Unary(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	ASSUME_BASE_TYPE(INK_NUMERIC);

	return new Ink_Numeric(-as<Ink_Numeric>(base)->value);
}

Ink_Object *InkNative_Numeric_ToString(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	stringstream ss;

	ASSUME_BASE_TYPE(INK_NUMERIC);
	ss << as<Ink_Numeric>(base)->value;

	return new Ink_String(string(ss.str()));
}

extern int numeric_native_method_table_count;
extern InkNative_MethodTable numeric_native_method_table[];

void Ink_Numeric::Ink_NumericMethodInit()
{
	InkNative_MethodTable *table = numeric_native_method_table;
	int i, count = numeric_native_method_table_count;

	for (i = 0; i < count; i++) {
		setSlot(table[i].name, table[i].func);
	}
}
