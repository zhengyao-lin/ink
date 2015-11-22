#include <sstream>
#include "../object.h"
#include "../context.h"

Ink_Object *InkNative_String_Add(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");

	if (argc && argv[0]->type == INK_STRING) {
		return new Ink_String(as<Ink_String>(base)->value + as<Ink_String>(argv[0])->value);
	} else if (argc && argv[0]->type == INK_INTEGER) {
		stringstream ss;
		ss << as<Ink_Integer>(argv[0])->value;
		return new Ink_String(as<Ink_String>(base)->value + ss.str());
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_String_Index(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");

	if (argc && argv[0]->type == INK_INTEGER) {
		int index = as<Ink_Integer>(argv[0])->value;
		return new Ink_String(as<Ink_String>(base)->value.substr(index, 1));
	}

	return new Ink_NullObject();
}

void Ink_String::Ink_StringMethodInit()
{
	setSlot("+", new Ink_FunctionObject(InkNative_String_Add));
	setSlot("[]", new Ink_FunctionObject(InkNative_String_Index));
}