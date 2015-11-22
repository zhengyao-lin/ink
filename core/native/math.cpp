#include "../object.h"
#include "../context.h"

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

void Ink_Integer::Ink_IntegerMethodInit()
{
	setSlot("+", new Ink_FunctionObject(InkNative_Integer_Add));
	setSlot("-", new Ink_FunctionObject(InkNative_Integer_Sub));
	setSlot("*", new Ink_FunctionObject(InkNative_Integer_Mul));
	setSlot("/", new Ink_FunctionObject(InkNative_Integer_Div));
	setSlot("+u", new Ink_FunctionObject(InkNative_Integer_Add_Unary));
	setSlot("-u", new Ink_FunctionObject(InkNative_Integer_Sub_Unary));
}