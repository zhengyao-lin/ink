#include "../object.h"
#include "../context.h"
#include "../expression.h"

extern Ink_ExpressionList native_exp_list;

Ink_Object *InkNative_Object_Index(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");

	if (argc && argv[0]->type == INK_STRING) {
		return Ink_HashExpression::getSlot(base, as<Ink_String>(argv[0])->value.c_str());
	}

	return new Ink_NullObject();
}

void Ink_Object::Ink_ObjectMethodInit()
{
	setSlot("[]", new Ink_FunctionObject(InkNative_Object_Index));
}