#include <vector>
#include "../object.h"
#include "../context.h"
#include "native.h"

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
		Ink_HashTable *hash;
		unsigned int index = as<Ink_Integer>(argv[0])->value;

		if (index < obj->value.size()) {
			if (!obj->value[index]) obj->value[index] = new Ink_HashTable("", new Ink_Undefined());
			for (hash = obj->value[index]; hash->bonding; hash = hash->bonding) ;
			hash->bondee = obj->value[index];
			ret = hash->value;
			ret->address = hash;
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

Ink_Object *InkNative_Array_Size(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	return new Ink_Integer(as<Ink_Array>(base)->value.size());
}

Ink_Object *InkNative_Array_Rebuild(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	Ink_Array *tmp;
	Ink_FunctionObject *tmp_func;
	Ink_ExpressionList ret_val;
	unsigned int i;

	if (base->type != INK_ARRAY) {
		InkWarn_Rebuild_Non_Array();
		return new Ink_NullObject();
	}

	tmp = as<Ink_Array>(base);
	ret_val = Ink_ExpressionList();
	for (i = 0; i < tmp->value.size(); i++) {
		if (tmp->value[i] && (tmp_func = as<Ink_FunctionObject>(tmp->value[i]->value))) {
			if (tmp_func->type == INK_FUNCTION) {
				ret_val.push_back(tmp_func->exp_list[0]);
			} else {
				InkWarn_Invalid_Element_For_Rebuild();
			}
		}
	}

	return new Ink_FunctionObject(Ink_ParamList(), ret_val, context->copyContextChain());
}

extern int array_native_method_table_count;
extern InkNative_MethodTable array_native_method_table[];

void Ink_Array::Ink_ArrayMethodInit()
{
	InkNative_MethodTable *table = array_native_method_table;
	int i, count = array_native_method_table_count;

	for (i = 0; i < count; i++) {
		setSlot(table[i].name, table[i].func);
	}
}