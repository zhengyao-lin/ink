#include <sstream>
#include "../object.h"
#include "../context.h"
#include "native.h"

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