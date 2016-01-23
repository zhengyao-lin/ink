#include <sstream>
#include "../object.h"
#include "../context.h"
#include "native.h"

inline string::size_type getRealIndex(long index, string::size_type size)
{
	while (index < 0) index += size;
	return index;
}

Ink_Object *InkNative_String_Add(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_String *tmp;

	ASSUME_BASE_TYPE(engine, INK_STRING);

	if (!checkArgument(engine, argc, 1)) {
		return NULL_OBJ;
	}

	if ((tmp = getStringVal(engine, context, argv[0])) != NULL) {
		return new Ink_String(engine, as<Ink_String>(base)->getValue() + tmp->getValue());
	}

	InkWarn_Invalid_Argument_For_String_Add(engine, argv[0]->type);
	return NULL_OBJ;
}

Ink_Object *InkNative_String_Index(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	string base_str;
	string::size_type index;

	ASSUME_BASE_TYPE(engine, INK_STRING);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		InkWarn_Method_Fallthrough(engine, INK_OBJECT);
		return InkNative_Object_Index(engine, context, argc, argv, this_p);
	}
	base_str = as<Ink_String>(base)->getValue();
	index = getRealIndex(as<Ink_Numeric>(argv[0])->value, base_str.length());

	return new Ink_String(engine, base_str.substr(index, 1));
}

Ink_Object *InkNative_String_Length(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);

	return new Ink_Numeric(engine, as<Ink_String>(base)->getValue().length());
}

Ink_Object *InkNative_String_SubStr(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	string::size_type offset;
	string::size_type length;

	ASSUME_BASE_TYPE(engine, INK_STRING);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	string origin = as<Ink_String>(base)->getValue();
	if (argc > 1 && argv[1]->type == INK_NUMERIC) {
		offset = getRealIndex(as<Ink_Numeric>(argv[0])->value, origin.length());
		length = as<Ink_Numeric>(argv[1])->value;
	} else {
		offset = getRealIndex(as<Ink_Numeric>(argv[0])->value, origin.length());
		length = string::npos;
	}

	if (offset >= origin.length()) {
		InkWarn_String_Index_Exceed(engine);
		return NULL_OBJ;
	} else if (!(length == string::npos || offset + length <= origin.length())) {
		InkWarn_Sub_String_Exceed(engine);
		return NULL_OBJ;
	}

	return new Ink_String(engine, origin.substr(offset, length));
}

Ink_Object *InkNative_String_Greater(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->getValue().c_str()[0]
								   > as<Ink_String>(argv[0])->getValue().c_str()[0]);
}

Ink_Object *InkNative_String_Less(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->getValue().c_str()[0] < as<Ink_String>(argv[0])->getValue().c_str()[0]);
}

Ink_Object *InkNative_String_GreaterOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->getValue().c_str()[0] >= as<Ink_String>(argv[0])->getValue().c_str()[0]);
}

Ink_Object *InkNative_String_LessOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_STRING);
	if (!checkArgument(engine, argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_String>(base)->getValue().c_str()[0] <= as<Ink_String>(argv[0])->getValue().c_str()[0]);
}

extern int string_native_method_table_count;
extern InkNative_MethodTable string_native_method_table[];

void Ink_String::Ink_StringMethodInit(Ink_InterpreteEngine *engine)
{
	InkNative_MethodTable *table = string_native_method_table;
	int i, count = string_native_method_table_count;

	for (i = 0; i < count; i++) {
		setSlot(table[i].name, table[i].func->cloneDeep(engine));
	}
}
