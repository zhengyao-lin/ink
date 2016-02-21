#include <math.h>
#include "../object.h"
#include "../context.h"
#include "native.h"

namespace ink {

using namespace std;

class Ink_Numeric;

Ink_Object *InkNative_Numeric_Add(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value + as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Sub(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value - as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Mul(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value * as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Div(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value / as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Mod(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, fmod(as<Ink_Numeric>(base)->value, as<Ink_Numeric>(argv[0])->value));
}

inline Ink_SInt64
getIntegerVal(Ink_Object *num /* assume numeric */)
{
	return (Ink_SInt64)(as<Ink_Numeric>(num)->value);
}

Ink_Object *InkNative_Numeric_And(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, getIntegerVal(base) & getIntegerVal(argv[0]));
}

Ink_Object *InkNative_Numeric_Or(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, getIntegerVal(base) | getIntegerVal(argv[0]));
}

Ink_Object *InkNative_Numeric_Xor(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, getIntegerVal(base) ^ getIntegerVal(argv[0]));
}

Ink_Object *InkNative_Numeric_ShiftLeft(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, getIntegerVal(base) << getIntegerVal(argv[0]));
}

Ink_Object *InkNative_Numeric_ShiftRight(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, getIntegerVal(base) >> getIntegerVal(argv[0]));
}

Ink_Object *InkNative_Numeric_Inverse(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, ~(getIntegerVal(base)));
}

Ink_Object *InkNative_Numeric_Equal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_NUMERIC)) {
		return InkNative_Object_Equal(engine, context, argc, argv, this_p);
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value == as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_NotEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	if (!argc) return NULL_OBJ;

	if (!checkArgument(false, argc, argv, 1, INK_NUMERIC)) {
		return InkNative_Object_NotEqual(engine, context, argc, argv, this_p);
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value != as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Greater(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value > as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Less(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value < as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_GreaterOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value >= as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_LessOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value <= as<Ink_Numeric>(argv[0])->value);
}

Ink_Object *InkNative_Numeric_Add_Unary(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->value);
}

Ink_Object *InkNative_Numeric_Sub_Unary(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, -as<Ink_Numeric>(base)->value);
}

Ink_Object *InkNative_Numeric_ToString(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	stringstream ss;

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);
	ss << as<Ink_Numeric>(base)->value;

	return new Ink_String(engine, string(ss.str()));
}

Ink_Object *InkNative_Numeric_Ceil(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, ceil(as<Ink_Numeric>(base)->value));
}

Ink_Object *InkNative_Numeric_Floor(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, floor(as<Ink_Numeric>(base)->value));
}

Ink_Object *InkNative_Numeric_Round(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, round(as<Ink_Numeric>(base)->value));
}

extern int numeric_native_method_table_count;
extern InkNative_MethodTable numeric_native_method_table[];

void Ink_Numeric::Ink_NumericMethodInit(Ink_InterpreteEngine *engine)
{
	InkNative_MethodTable *table = numeric_native_method_table;
	Ink_Object *tmp;
	int i, count = numeric_native_method_table_count;

	for (i = 0; i < count; i++) {
		setSlot(table[i].name, tmp = table[i].func->cloneDeep(engine));
		tmp->initProto(engine);
	}
}

}
