#include <vector>
#include "native.h"
#include "../object.h"
#include "../context.h"
#include "../interface/engine.h"
#include "../gc/collect.h"

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
		InkNote_Method_Fallthrough(engine, "[]", INK_ARRAY, INK_OBJECT);
		return InkNative_Object_Index(engine, context, argc, argv, this_p);
	}

	index = getRealIndex(as<Ink_Numeric>(argv[0])->value, obj->value.size());
	if (index < obj->value.size()) {
		if (!obj->value[index]) obj->value[index] = new Ink_HashTable(UNDEFINED);
		hash = Ink_Object::traceHashBond(obj->value[index]);
		ret = hash->getValue();
		ret->address = hash;
		// ret->setSlot_c("base", base);
		ret->setBase(base);
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
	Ink_Array *ret = NULL;
	Ink_ArrayValue::size_type i;
	IGC_CollectEngine *gc_engine = engine->getCurrentGC();

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (!checkArgument(engine, argc, argv, 1, INK_FUNCTION)) {
		return NULL_OBJ;
	}

	ret = new Ink_Array(engine);
	engine->addPardonObject(ret);

	args = (Ink_Object **)malloc(sizeof(Ink_Object *));
	for (i = 0; i < array->value.size(); i++) {
		gc_engine->checkGC();

		args[0] = array->value[i] ? array->value[i]->getValue() : UNDEFINED;
		ret->value.push_back(new Ink_HashTable(ret_tmp = argv[0]->call(engine, context, 1, args)));
		if (engine->getSignal() != INTER_NONE) {
			switch (engine->getSignal()) {
				case INTER_RETURN:
					free(args);
					engine->removePardonObject(ret);
					return engine->getInterruptValue(); // signal penetrated
				case INTER_DROP:
				case INTER_BREAK:
					free(args);
					engine->removePardonObject(ret);
					return engine->trapSignal(); // trap the signal
				case INTER_CONTINUE:
					engine->trapSignal(); // trap the signal, but do not return
					continue;
				default:
					free(args);
					engine->removePardonObject(ret);
					return NULL_OBJ;
			}
		}
	}
	free(args);
	engine->removePardonObject(ret);

	return ret;
}

Ink_Object *InkNative_Array_Last(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	Ink_ArrayValue tmp = as<Ink_Array>(base)->value;
	Ink_Object *ret = NULL;
	Ink_HashTable *ret_hash = tmp[tmp.size() - 1];

	if (!ret_hash) {
		ret_hash = tmp[tmp.size() - 1] = new Ink_HashTable(ret = UNDEFINED);
	} else {
		ret = ret_hash->getValue();
	}

	ret->address = ret_hash;

	return ret;
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
	Ink_Object *tmp_obj;
	Ink_FunctionObject *tmp_func;
	Ink_ExpListObject *tmp_exp_obj;

	Ink_ExpressionList ret_val;
	Ink_ArrayValue::size_type i;

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	tmp = as<Ink_Array>(base);
	ret_val = Ink_ExpressionList();
	for (i = 0; i < tmp->value.size(); i++) {
		if (tmp->value[i] && (tmp_obj = tmp->value[i]->getValue())) {
			if (tmp_obj->type == INK_FUNCTION) {
				tmp_func = as<Ink_FunctionObject>(tmp_obj);
				ret_val.insert(ret_val.end(),
							   tmp_func->exp_list.begin(),
							   tmp_func->exp_list.end());
			} else if (tmp_obj->type == INK_EXPLIST) {
				tmp_exp_obj = as<Ink_ExpListObject>(tmp_obj);
				ret_val.insert(ret_val.end(),
							   tmp_exp_obj->exp_list.begin(),
							   tmp_exp_obj->exp_list.end());
			} else {
				InkWarn_Invalid_Element_For_Rebuild(engine);
			}
		}
	}

	return new Ink_FunctionObject(engine, Ink_ParamList(), ret_val, context->copyContextChain());
}

void Ink_Array::Ink_ArrayMethodInit(Ink_InterpreteEngine *engine)
{
	setSlot_c("[]", new Ink_FunctionObject(engine, InkNative_Array_Index));
	setSlot_c("push", new Ink_FunctionObject(engine, InkNative_Array_Push));
	setSlot_c("size", new Ink_FunctionObject(engine, InkNative_Array_Size));
	setSlot_c("each", new Ink_FunctionObject(engine, InkNative_Array_Each, true));
	setSlot_c("last", new Ink_FunctionObject(engine, InkNative_Array_Last));
	setSlot_c("remove", new Ink_FunctionObject(engine, InkNative_Array_Remove));
	setSlot_c("rebuild", new Ink_FunctionObject(engine, InkNative_Array_Rebuild));
	
	return;
}

}
