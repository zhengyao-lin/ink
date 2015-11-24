#include "../object.h"
#include "../context.h"
#include "../expression.h"
#include "../error.h"

Ink_Object *InkNative_Function_Insert(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	Ink_Object *arg = argv[0];

	if (base->type == INK_FUNCTION && arg->type == INK_FUNCTION) {
		Ink_FunctionObject *func = as<Ink_FunctionObject>(base);
		Ink_FunctionObject *insert = as<Ink_FunctionObject>(arg);
		func->exp_list.insert(func->exp_list.end(), insert->exp_list.begin(), insert->exp_list.end());
		return func;
	}

	InkWarn_Insert_Non_Function_Object();
	return new Ink_NullObject();
}

void Ink_FunctionObject::Ink_FunctionMethodInit()
{
	setSlot("<<", new Ink_FunctionObject(InkNative_Function_Insert));
}