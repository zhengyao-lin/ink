#include <vector>
#include "../object.h"
#include "../context.h"
#include "native.h"

unsigned int getRealIndex(int index, int size)
{
	while (index < 0) index += size;
	return index;
}

Ink_Object *InkNative_Array_Index(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	if (base->type != INK_ARRAY) {
		InkWarn_Get_Non_Array_Index();
		return new Ink_NullObject();
	}

	if (argc && argv[0]->type == INK_NUMERIC) {
		Ink_Array *obj = as<Ink_Array>(base);
		Ink_Object *ret;
		Ink_HashTable *hash;
		unsigned int index = getRealIndex(as<Ink_Numeric>(argv[0])->value, obj->value.size());

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

Ink_Object *InkNative_Array_Push(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	if (argc) {
		Ink_Array *obj = as<Ink_Array>(base);
		obj->value.push_back(new Ink_HashTable("", argv[0]));
		return argv[0];
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Array_Size(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	return new Ink_Numeric(as<Ink_Array>(base)->value.size());
}

void cleanArrayHashTable(Ink_ArrayValue val, int begin, int end) // assume that begin <= end
{
	int index;
	for (index = begin; index < end; index++) {
		delete val[index];
	}
	return;
}

Ink_Object *InkNative_Array_Remove(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	// Ink_Object *ret;
	Ink_Array *tmp;
	unsigned int index_begin, index_end, tmp_val;

	if (!argc || argv[0]->type != INK_NUMERIC) {
		InkWarn_Remove_Argument_Require();
		return new Ink_NullObject();
	}

	tmp = as<Ink_Array>(base);
	index_begin = getRealIndex(as<Ink_Numeric>(argv[0])->value,
							   tmp->value.size());

	if (argc > 1 && argv[1]->type == INK_NUMERIC) {
		index_end = getRealIndex(as<Ink_Numeric>(argv[1])->value,
								 tmp->value.size());
	} else index_end = index_begin;

	if (index_end > tmp->value.size() || index_end > tmp->value.size()) {
		InkWarn_Too_Huge_Index();
		return new Ink_NullObject();
	}

	if (index_end != index_begin) {
		if (index_begin > index_end) {
			tmp_val = index_begin;
			index_begin = index_end;
			index_end = tmp_val;
		}
		index_end++;
		cleanArrayHashTable(tmp->value, index_begin, index_end);
		tmp->value.erase(tmp->value.begin() + index_begin,
						 tmp->value.begin() + index_end);
	} else {
		delete tmp->value[index_begin];
		tmp->value.erase(tmp->value.begin() + index_begin);
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Array_Rebuild(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
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