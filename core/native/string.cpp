#include <sstream>
#include "../object.h"
#include "../context.h"
#include "native.h"

Ink_Object *InkNative_String_Add(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	if (checkArgument(false, argc, argv, 1, INK_STRING)) {
		return new Ink_String(as<Ink_String>(base)->value + as<Ink_String>(argv[0])->value);
	} else if (checkArgument(argc, argv, 1, INK_NUMERIC)) {
		stringstream ss;
		ss << as<Ink_Numeric>(argv[0])->value;
		return new Ink_String(as<Ink_String>(base)->value + ss.str());
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_String_Index(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	int index;

	if (!checkArgument(argc, argv, 1, INK_NUMERIC)) {
		InkWarn_Method_Fallthrough(INK_OBJECT);
		return InkNative_Object_Index(context, argc, argv, this_p);
	}
	index = as<Ink_Numeric>(argv[0])->value;

	return new Ink_String(as<Ink_String>(base)->value.substr(index, 1));
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