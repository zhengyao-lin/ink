#include <vector>
#include "../object.h"
#include "../context.h"
#include "native.h"
#include "../interface/engine.h"

namespace ink {

inline Ink_ArrayValue::size_type getRealIndex(long index, Ink_ArrayValue::size_type size)
{
	while (index < 0) index += size;
	return index;
}

Ink_Object *InkNative_Array_Index(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_Array *obj = as<Ink_Array>(base);
	Ink_Object *ret;
	Ink_HashTable *hash;
	Ink_ArrayValue::size_type index;

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (!checkArgument(false, argc, argv, 1, INK_NUMERIC)) {
		return InkNative_Object_Index(engine, context, argc, argv, this_p);
	}

	index = getRealIndex(as<Ink_Numeric>(argv[0])->value, obj->value.size());
	if (index < obj->value.size()) {
		if (!obj->value[index]) obj->value[index] = new Ink_HashTable(UNDEFINED);
		for (hash = obj->value[index]; hash->bonding; hash = hash->bonding) ;
		hash->bondee = obj->value[index];
		ret = hash->getValue();
		ret->address = hash;
		ret->setSlot("base", base);
	} else {
		InkWarn_Index_Exceed(engine);
		return UNDEFINED;
	}

	return ret;
}

Ink_Object *InkNative_Array_Push(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (argc) {
		Ink_Array *obj = as<Ink_Array>(base);
		obj->value.push_back(new Ink_HashTable(argv[0]));
		return argv[0];
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_Array_Size(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	ASSUME_BASE_TYPE(engine, INK_ARRAY);
	return new Ink_Numeric(engine, as<Ink_Array>(base)->value.size());
}

void cleanArrayHashTable(Ink_ArrayValue val,
						 Ink_ArrayValue::size_type begin,
						 Ink_ArrayValue::size_type end) // assume that begin <= end
{
	Ink_ArrayValue::size_type index;
	for (index = begin; index < end; index++) {
		delete val[index];
	}
	return;
}

Ink_Object *InkNative_Array_Each(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_Array *array = as<Ink_Array>(base);
	Ink_Object **args;
	Ink_Object *ret_tmp;
	Ink_ArrayValue ret_val;
	Ink_ArrayValue::size_type i;

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (!checkArgument(engine, argc, argv, 1, INK_FUNCTION)) {
		return NULL_OBJ;
	}

	args = (Ink_Object **)malloc(sizeof(Ink_Object *));
	for (i = 0; i < array->value.size(); i++) {
		args[0] = array->value[i] ? array->value[i]->getValue() : UNDEFINED;
		ret_val.push_back(new Ink_HashTable(ret_tmp = argv[0]->call(engine, context, 1, args)));
		switch (engine->getSignal()) {
			case INTER_RETURN:
				free(args);
				cleanArrayHashTable(ret_val);
				return engine->getInterruptValue(); // signal penetrated
			case INTER_DROP:
			case INTER_BREAK:
				free(args);
				cleanArrayHashTable(ret_val);
				return engine->trapSignal(); // trap the signal
			case INTER_CONTINUE:
				engine->trapSignal(); // trap the signal, but do not return
				continue;
			default: ;
		}
	}
	free(args);

	return new Ink_Array(engine, ret_val);
}

Ink_Object *InkNative_Array_Remove(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	// Ink_Object *ret;
	Ink_Array *tmp;
	Ink_Object *ret = NULL_OBJ;
	Ink_ArrayValue::size_type index_begin, index_end, tmp_val;

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	tmp = as<Ink_Array>(base);
	index_begin = getRealIndex(as<Ink_Numeric>(argv[0])->value,
							   tmp->value.size());

	if (argc > 1 && argv[1]->type == INK_NUMERIC) {
		index_end = getRealIndex(as<Ink_Numeric>(argv[1])->value,
								 tmp->value.size());
	} else index_end = index_begin;

	if (index_end > tmp->value.size() || index_end > tmp->value.size()) {
		InkWarn_Too_Huge_Index(engine);
		return NULL_OBJ;
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
		if (tmp->value[index_begin]) ret = tmp->value[index_begin]->getValue();
		delete tmp->value[index_begin];
		tmp->value.erase(tmp->value.begin() + index_begin);
	}

	return ret;
}

Ink_Object *InkNative_Array_Rebuild(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_Array *tmp;
	Ink_FunctionObject *tmp_func;
	Ink_ExpressionList ret_val;
	Ink_ArrayValue::size_type i;

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	tmp = as<Ink_Array>(base);
	ret_val = Ink_ExpressionList();
	for (i = 0; i < tmp->value.size(); i++) {
		if (tmp->value[i] && (tmp_func = as<Ink_FunctionObject>(tmp->value[i]->getValue()))) {
			if (tmp_func->type == INK_FUNCTION) {
				ret_val.push_back(tmp_func->exp_list[0]);
			} else {
				InkWarn_Invalid_Element_For_Rebuild(engine);
			}
		}
	}

	return new Ink_FunctionObject(engine, Ink_ParamList(), ret_val, context->copyContextChain());
}

extern int array_native_method_table_count;
extern InkNative_MethodTable array_native_method_table[];

void Ink_Array::Ink_ArrayMethodInit(Ink_InterpreteEngine *engine)
{
	InkNative_MethodTable *table = array_native_method_table;
	int i, count = array_native_method_table_count;

	for (i = 0; i < count; i++) {
		setSlot(table[i].name, table[i].func->cloneDeep(engine));
	}
}

}
