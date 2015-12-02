#include "../object.h"
#include "../context.h"
#include "native.h"

class Ink_Integer;

Ink_Object *InkNative_Integer_Add(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER && argv[0]->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value + as<Ink_Integer>(argv[0])->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_Sub(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER && argv[0]->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value - as<Ink_Integer>(argv[0])->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_Mul(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER && argv[0]->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value * as<Ink_Integer>(argv[0])->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_Div(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER && argv[0]->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value / as<Ink_Integer>(argv[0])->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_Equal(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER && argc && argv[0]->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value == as<Ink_Integer>(argv[0])->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_NotEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER && argc && argv[0]->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value != as<Ink_Integer>(argv[0])->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_Greater(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER && argc && argv[0]->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value > as<Ink_Integer>(argv[0])->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_Less(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER && argc && argv[0]->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value < as<Ink_Integer>(argv[0])->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_GreaterOrEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER && argc && argv[0]->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value >= as<Ink_Integer>(argv[0])->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_LessOrEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER && argc && argv[0]->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value <= as<Ink_Integer>(argv[0])->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_Add_Unary(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER) {
		return new Ink_Integer(as<Ink_Integer>(base)->value);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Integer_Sub_Unary(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	if (base->type == INK_INTEGER) {
		return new Ink_Integer(-as<Ink_Integer>(base)->value);
	}

	return new Ink_NullObject();
}

extern int integer_native_method_table_count;
extern InkNative_MethodTable integer_native_method_table[];

void Ink_Integer::Ink_IntegerMethodInit()
{
	InkNative_MethodTable *table = integer_native_method_table;
	int i, count = integer_native_method_table_count;

	for (i = 0; i < count; i++) {
		setSlot(table[i].name, table[i].func);
	}
}