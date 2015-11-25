#include <vector>
#include "../object.h"
#include "../context.h"

Ink_Object *InkNative_Array_Index(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");

	if (base->type != INK_ARRAY) {
		InkWarn_Get_Non_Array_Index();
		return new Ink_NullObject();
	}

	if (argc && argv[0]->type == INK_INTEGER) {
		Ink_Array *obj = as<Ink_Array>(base);
		Ink_Object *ret;
		unsigned int index = as<Ink_Integer>(argv[0])->value;

		if (index < obj->value.size()) {
			if (!obj->value[index]) obj->value[index] = new Ink_HashTable("", new Ink_Undefined());
			ret = obj->value[index]->value;
			ret->address = obj->value[index];
		} else {
			InkWarn_Index_Exceed();
			return new Ink_Undefined();
		}

		return ret;
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Array_Push(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");

	if (argc) {
		Ink_Array *obj = as<Ink_Array>(base);
		obj->value.push_back(new Ink_HashTable("", argv[0]));
		return argv[0];
	}

	return new Ink_NullObject();
}

void Ink_Array::Ink_ArrayMethodInit()
{
	setSlot("[]", new Ink_FunctionObject(InkNative_Array_Index));
	setSlot("push", new Ink_FunctionObject(InkNative_Array_Push));
}