#include "../object.h"
#include "../context.h"
#include "../expression.h"
#include "../error.h"
#include "native.h"

Ink_Object *InkNative_Function_Insert(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_ArgcType i;

	ASSUME_BASE_TYPE(engine, INK_FUNCTION);

	if (!checkArgument(false, argc, argv, 1, INK_FUNCTION)) {
		InkWarn_Insert_Non_Function_Object(engine);
		return NULL_OBJ;
	}

	Ink_FunctionObject *func = as<Ink_FunctionObject>(base);
	Ink_FunctionObject *insert;

	for (i = 0; i < argc; i++) {
		if (argv[i]->type != INK_FUNCTION) {
			InkWarn_Insert_Non_Function_Object(engine);
			return func;
		}
		insert = as<Ink_FunctionObject>(argv[i]);
		func->exp_list.insert(func->exp_list.end(),
							  insert->exp_list.begin(),
							  insert->exp_list.end());
	}

	return func;
}

Ink_Object **arrayValueToObject(Ink_ArrayValue val)
{
	Ink_Object **ret = (Ink_Object **)malloc(sizeof(Ink_Object *) * val.size());
	Ink_ArrayValue::size_type i;

	for (i = 0; i < val.size(); i++) {
		ret[i] = val[i]->getValue();
	}

	return ret;
}

Ink_Object *InkNative_Function_RangeCall(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_Object *range;
	Ink_ArrayValue range_val, tmp_arr_val;
	Ink_ArrayValue ret_val;
	Ink_Object **tmp;
	Ink_ArrayValue::size_type i;

	ASSUME_BASE_TYPE(engine, INK_FUNCTION);

	if (!argc) {
		InkWarn_Function_Range_Call_Argument_Error(engine);
		return NULL_OBJ;
	}

	range = getSlotWithProto(engine, context, argv[0], "range");
	if (range->type != INK_FUNCTION) {
		InkWarn_Method_Fallthrough(engine, INK_OBJECT);
		return InkNative_Object_Index(engine, context, argc, argv, this_p);
	}

	range = range->call(engine, context);
	if (range->type != INK_ARRAY) {
		InkWarn_Incorrect_Range_Type(engine);
		return NULL_OBJ;
	}

	ret_val = Ink_ArrayValue();
	range_val = as<Ink_Array>(range)->value;

	for (i = 0; i < range_val.size(); i++) {
		if (range_val[i]
			&& range_val[i]->getValue()->type == INK_ARRAY) {
			tmp_arr_val = as<Ink_Array>(range_val[i]->getValue())->value;
			tmp = arrayValueToObject(tmp_arr_val);
			ret_val.push_back(new Ink_HashTable("", base->call(engine, context, tmp_arr_val.size(), tmp)));
			free(tmp);
		} else {
			InkWarn_Incorrect_Range_Type(engine);
			return NULL_OBJ;
		}
	}

	return new Ink_Array(engine, ret_val);
}

Ink_Object *InkNative_Function_GetExp(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_FunctionObject *tmp;
	Ink_ExpressionList tmp_exp;
	Ink_ArrayValue ret_val;
	Ink_ExpressionList::size_type i;

	ASSUME_BASE_TYPE(engine, INK_FUNCTION);

	tmp = as<Ink_FunctionObject>(base);
	for (i = 0; i < tmp->exp_list.size(); i++) {
		tmp_exp = Ink_ExpressionList();
		tmp_exp.push_back(tmp->exp_list[i]);
		ret_val.push_back(new Ink_HashTable("", new Ink_FunctionObject(engine, Ink_ParamList(), tmp_exp,
																	   tmp->closure_context->copyContextChain(), true)));
	}

	return new Ink_Array(engine, ret_val);
}

Ink_Object *InkNative_Function_GetScope(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_FunctionObject *func, *expr;
	Ink_Object *ret;

	ASSUME_BASE_TYPE(engine, INK_FUNCTION);

	if (!checkArgument(engine, argc, argv, 1, INK_FUNCTION)) {
		return NULL_OBJ;
	}

	func = as<Ink_FunctionObject>(base);
	expr = as<Ink_FunctionObject>(argv[0]);

	if (func->closure_context && expr->exp_list.size()) {
		ret = expr->exp_list[0]->eval(engine, func->closure_context);
	} else {
		ret = expr->exp_list[0]->eval(engine, context);
	}

	return ret;
}

extern int function_native_method_table_count;
extern InkNative_MethodTable function_native_method_table[];

void Ink_FunctionObject::Ink_FunctionMethodInit()
{
	InkNative_MethodTable *table = function_native_method_table;
	int i, count = function_native_method_table_count;

	for (i = 0; i < count; i++) {
		setSlot(table[i].name, table[i].func);
	}
}
