#include "../object.h"
#include "../context.h"
#include "../numeric.h"
#include "native.h"

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

	return new Ink_Numeric(engine, (int)as<Ink_Numeric>(base)->value % (int)as<Ink_Numeric>(argv[0])->value);
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

// Big Numeric

Ink_Object *InkNative_BigNumeric_Add(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value + val);
}

Ink_Object *InkNative_BigNumeric_Sub(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value - val);
}

Ink_Object *InkNative_BigNumeric_Mul(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value * val);
}

Ink_Object *InkNative_BigNumeric_Div(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;
	Ink_BigNumericValue ret = 0;
	if (argc > 1 && argv[1]->type == INK_NUMERIC) {
		ret = as<Ink_BigNumeric>(base)->value.dividedBy(val, as<Ink_Numeric>(argv[1])->value);
	} else {
		ret = as<Ink_BigNumeric>(base)->value / val;
	}

	return new Ink_BigNumeric(engine, ret);
}

Ink_Object *InkNative_BigNumeric_Mod(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value % val);
}

Ink_Object *InkNative_BigNumeric_Equal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value == val);
}

Ink_Object *InkNative_BigNumeric_NotEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value != val);
}

Ink_Object *InkNative_BigNumeric_Greater(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value > val);
}

Ink_Object *InkNative_BigNumeric_Less(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value < val);
}

Ink_Object *InkNative_BigNumeric_GreaterOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value >= val);
}

Ink_Object *InkNative_BigNumeric_LessOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_BIGNUMERIC)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_BigNumericValue val = argv[0]->type == INK_NUMERIC
							  ? as<Ink_Numeric>(argv[0])->value
							  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value <= val);
}

Ink_Object *InkNative_BigNumeric_Add_Unary(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value);
}

Ink_Object *InkNative_BigNumeric_Sub_Unary(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	return new Ink_BigNumeric(engine, -as<Ink_BigNumeric>(base)->value);
}

Ink_Object *InkNative_BigNumeric_ToString(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_BIGNUMERIC);

	return new Ink_String(engine, as<Ink_BigNumeric>(base)->value.toString());
}	