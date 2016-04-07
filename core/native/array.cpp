#include <vector>
#include "native.h"
#include "../object.h"
#include "../context.h"
#include "../interface/engine.h"
#include "../gc/collect.h"

namespace ink {
	
inline Ink_ArrayValue::size_type getRealIndex(Ink_SInt64 index, Ink_ArrayValue::size_type size)
{
	while (index < 0) index += size;
	return index;
}

inline Ink_ArrayValue::size_type getRealIndex(Ink_NumericValue val, Ink_ArrayValue::size_type size)
{
	Ink_SInt64 index = getInt(val);
	while (index < 0) index += size;
	return index;
}

Ink_Object *InkNative_Array_Link(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_ArrayValue::size_type i, size;

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (!checkArgument(engine, argc, argv, 1, INK_ARRAY)) {
		return NULL_OBJ;
	}

	Ink_ArrayValue base_val = as<Ink_Array>(base)->value;
	Ink_ArrayValue linkee = as<Ink_Array>(argv[0])->value;
	Ink_ArrayValue ret_val = Ink_ArrayValue(size = base_val.size() + linkee.size(), NULL);
	Ink_Array *ret = new Ink_Array(engine, ret_val);

	for (i = 0; i < size; i++) {
		if (i < base_val.size()) {
			if (base_val[i]) {
				ret->value[i] = new Ink_HashTable(base_val[i]->getValue(), ret);
			}
		} else {
			if (linkee[i - base_val.size()]) {
				ret->value[i] = new Ink_HashTable(linkee[i - base_val.size()]->getValue(), ret);
			}
		}
	}

	return ret;
}

Ink_Object *InkNative_Array_Index(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Array *obj = as<Ink_Array>(base);
	Ink_Object *ret;
	Ink_HashTable *hash;
	Ink_ArrayValue::size_type index;

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (argc > 1) {
		/* two or more arguments -- slice */
		InkNote_Method_Fallthrough(engine, "[]", "slice", INK_ARRAY);
		return InkNative_Array_Slice(engine, context, base, argc, argv, this_p);
	}

	if (!checkArgument(false, argc, argv, 1, INK_NUMERIC)) {
		InkNote_Method_Fallthrough(engine, "[]", INK_ARRAY, INK_OBJECT);
		return InkNative_Object_Index(engine, context, base, argc, argv, this_p);
	}

	index = getRealIndex(as<Ink_Numeric>(argv[0])->getValue(), obj->value.size());
	if (index < obj->value.size()) {
		if (!obj->value[index]) obj->value[index] = new Ink_HashTable(UNDEFINED, obj);
		hash = Ink_Object::traceHashBond(obj->value[index]);
		ret = hash->getValue();
		ret->address = hash;
		// ret->setSlot_c("base", base);
		ret->setBase(base);
	} else {
		InkWarn_Array_Index_Exceed(engine, index, obj->value.size());
		return UNDEFINED;
	}

	return ret;
}

Ink_Object *InkNative_Array_Push(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (argc) {
		Ink_Array *obj = as<Ink_Array>(base);
		obj->value.push_back(new Ink_HashTable(argv[0], obj));
		return argv[0];
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_Array_Size(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
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

Ink_Object *InkNative_Array_Each(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Array *base_arr;
	Ink_Object **args;
	Ink_Array *ret = NULL;
	Ink_ArrayValue::size_type i;
	IGC_CollectEngine *gc_engine = engine->getCurrentGC();

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (!checkArgument(engine, argc, argv, 1, INK_FUNCTION)) {
		return NULL_OBJ;
	}

	base_arr = as<Ink_Array>(base);

	ret = new Ink_Array(engine);
	engine->addPardonObject(ret);

	args = (Ink_Object **)malloc(sizeof(Ink_Object *));
	for (i = 0; i < base_arr->value.size(); i++) {
		gc_engine->checkGC();

		args[0] = base_arr->value[i] ? base_arr->value[i]->getValue() : UNDEFINED;
		ret->value.push_back(new Ink_HashTable(argv[0]->call(engine, context, base, 1, args), ret));
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

Ink_Object *InkNative_Array_Zip(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object **args;
	Ink_Object *block = NULL;

	Ink_Array *base_arr = NULL;
	Ink_Array *zipee = NULL;
	Ink_Array *ret = NULL, *tmp = NULL;

	Ink_ArrayValue::size_type i;
	IGC_CollectEngine *gc_engine = engine->getCurrentGC();

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (!checkArgument(engine, argc, argv, 1, INK_ARRAY)) {
		return NULL_OBJ;
	}

	if (argc > 1 && argv[1]->type == INK_FUNCTION) {
		block = argv[1];
	}

	base_arr = as<Ink_Array>(base);
	zipee = as<Ink_Array>(argv[0]);

	ret = new Ink_Array(engine);
	engine->addPardonObject(ret);
	engine->addPardonObject(zipee);
	// note: block doesn't need to be added to pardon list cause it can only be created in outer GC engine

	args = (Ink_Object **)malloc(sizeof(Ink_Object *) * 2);
	for (i = 0; i < base_arr->value.size(); i++) {
		gc_engine->checkGC();

		args[0] = base_arr->value[i] ? base_arr->value[i]->getValue() : UNDEFINED;
		args[1] = (i < zipee->value.size() && zipee->value[i])
				  ? zipee->value[i]->getValue() : UNDEFINED;

		if (block) {
			ret->value.push_back(new Ink_HashTable(argv[1]->call(engine, context, base, 2, args), ret));
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
		} else {
			ret->value.push_back(new Ink_HashTable(tmp = new Ink_Array(engine), ret));
			tmp->value.push_back(new Ink_HashTable(args[0], tmp));
			tmp->value.push_back(new Ink_HashTable(args[1], tmp));
		}
	}
	free(args);
	engine->removePardonObject(ret);
	engine->removePardonObject(zipee);

	return ret;
}

Ink_Object *InkNative_Array_Build(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (!checkArgument(engine, argc, argv, 1, INK_FUNCTION)) {
		return NULL_OBJ;
	}

	Ink_Array *base_val = as<Ink_Array>(base);
	Ink_ArrayValue::size_type i;
	Ink_Object *ret = base_val->value.size() ? base_val->value[0]->getValue() : NULL;
	Ink_Object *ret_back;
	Ink_FunctionObject *build_fn = as<Ink_FunctionObject>(argv[0]);
	Ink_Object **tmp_argv = (Ink_Object **)malloc(sizeof(Ink_Object *) * 2);

	for (i = 1; i < base_val->value.size(); i++) {
		tmp_argv[0] = ret;
		tmp_argv[1] = base_val->value[i] ? base_val->value[i]->getValue() : UNDEFINED;
		engine->addPardonObject(ret_back = ret);
		ret = build_fn->call(engine, context, 2, tmp_argv);
		engine->removePardonObject(ret_back);
	}

	free(tmp_argv);

	return ret ? ret : NULL_OBJ;
}

Ink_Object *InkNative_Array_Last(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	Ink_ArrayValue tmp = as<Ink_Array>(base)->value;
	Ink_Object *ret = NULL;
	Ink_HashTable *ret_hash = tmp[tmp.size() - 1];

	if (!ret_hash) {
		ret_hash = tmp[tmp.size() - 1] = new Ink_HashTable(ret = UNDEFINED, base);
	} else {
		ret = ret_hash->getValue();
	}

	ret->address = ret_hash;

	return ret;
}

Ink_Object *InkNative_Array_Remove(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	// Ink_Object *ret;
	Ink_Array *tmp;
	Ink_Object *ret = NULL_OBJ;
	Ink_ArrayValue::size_type index_begin, index_end, tmp_val;

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	tmp = as<Ink_Array>(base);
	index_begin = getRealIndex(as<Ink_Numeric>(argv[0])->getValue(),
							   tmp->value.size());

	if (argc > 1 && argv[1]->type == INK_NUMERIC) {
		index_end = getRealIndex(as<Ink_Numeric>(argv[1])->getValue(),
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

Ink_Object *InkNative_Array_Slice(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{

	ASSUME_BASE_TYPE(engine, INK_ARRAY);

	Ink_ArrayValue base_val = as<Ink_Array>(base)->value;
	Ink_ArrayValue::size_type start = 0, end = base_val.size() - 1, tmp, i;
	Ink_SInt64 range = 1;

	if (argc > 0) {
		if (argv[0]->type == INK_NUMERIC) {
			start = getRealIndex(as<Ink_Numeric>(argv[0])->getValue(), base_val.size());
		} else if (argv[0]->type != INK_UNDEFINED) {
			InkWarn_Slice_Require_Numeric(engine, 1);
		}

		if (argc > 1) {
			if (argv[1]->type == INK_NUMERIC) {
				end = getRealIndex(as<Ink_Numeric>(argv[1])->getValue(), base_val.size());
			} else if (argv[1]->type != INK_UNDEFINED) {
				InkWarn_Slice_Require_Numeric(engine, 2);
			}

			if (argc > 2) {
				if (argv[2]->type == INK_NUMERIC) {
					range = getInt(as<Ink_Numeric>(argv[2])->getValue());
				} else if (argv[2]->type != INK_UNDEFINED) {
					InkWarn_Slice_Require_Numeric(engine, 3);
				}
			}
		}
	}

	if (start > end) {
		InkNote_Slice_Start_Greater(engine, start, end);
		tmp = start;
		start = end;
		end = tmp;
	}

	if (!range) {
		InkWarn_Slice_Require_Non_Zero_Range(engine);
		range = 1;
	}

	if (end >= base_val.size()) {
		InkWarn_Array_Index_Exceed(engine, end, base_val.size());
		return NULL_OBJ;
	}

	Ink_Array *ret = new Ink_Array(engine);

	if (range > 0) {
		for (i = start; i <= end; i += range) {
			if (base_val[i]) {
				ret->value.push_back(new Ink_HashTable(base_val[i]->getValue(), ret));
			} else {
				ret->value.push_back(NULL);
			}
		}
	} else {
		for (i = end; i >= start;) {
			if (base_val[i]) {
				ret->value.push_back(new Ink_HashTable(base_val[i]->getValue(), ret));
			} else {
				ret->value.push_back(NULL);
			}

			if ((Ink_UInt64)-range > i) {
				break;
			}
			i += range;
		}
	}

	return ret;
}

Ink_Object *InkNative_Array_Rebuild(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_Object *base, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
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
	setSlot_c("+", new Ink_FunctionObject(engine, InkNative_Array_Link));
	setSlot_c("[]", new Ink_FunctionObject(engine, InkNative_Array_Index));
	setSlot_c("push", new Ink_FunctionObject(engine, InkNative_Array_Push));
	setSlot_c("size", new Ink_FunctionObject(engine, InkNative_Array_Size));
	setSlot_c("each", new Ink_FunctionObject(engine, InkNative_Array_Each, true));
	setSlot_c("zip", new Ink_FunctionObject(engine, InkNative_Array_Zip, true));
	setSlot_c("build", new Ink_FunctionObject(engine, InkNative_Array_Build));
	setSlot_c("last", new Ink_FunctionObject(engine, InkNative_Array_Last));
	setSlot_c("remove", new Ink_FunctionObject(engine, InkNative_Array_Remove));
	setSlot_c("slice", new Ink_FunctionObject(engine, InkNative_Array_Slice));
	setSlot_c("rebuild", new Ink_FunctionObject(engine, InkNative_Array_Rebuild));
	
	return;
}

}
