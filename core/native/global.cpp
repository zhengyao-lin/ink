#include <vector>
#include "../object.h"
#include "../context.h"
#include "../expression.h"
#include "../error.h"
#include "native.h"

extern Ink_ExpressionList native_exp_list;
extern bool CGC_if_return;

void disposeParamList(Ink_ParamList param)
{
	unsigned int i;
	for (i = 0; i < param.size(); i++) {
		delete param[i];
	}
	return;
}

Ink_Expression *getEmptyBlockFunction()
{
	Ink_ParamList ret_param;

	ret_param = Ink_ParamList();
	ret_param.push_back(new string("block"));

	return new Ink_FunctionExpression(ret_param, Ink_ExpressionList(), true);
}

bool isTrue(Ink_Object *cond)
{
	if (cond->type == INK_INTEGER) {
		if (as<Ink_Integer>(cond)->value)
			return true;
		return false;
	}

	return cond->type != INK_NULL && cond->type != INK_UNDEFINED;
}

Ink_Object *Ink_IfExpression(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *cond;
	Ink_Object *ret;

	if (!argc) {
		InkWarn_If_Argument_Fault();
		return new Ink_NullObject();
	}

	ret = cond = argv[0];
	if (isTrue(cond)) {
		if (argc > 1 && argv[1]->type == INK_FUNCTION) {
			ret = argv[1]->call(context);
		}
	} else {
		if (argc > 2 && argv[2]->type == INK_FUNCTION) {
			ret = argv[2]->call(context);
		}
	}

	return ret;
}

Ink_Object *Ink_WhileExpression(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *cond;
	Ink_Object *block;
	Ink_Object *ret;

	if (argc < 2) {
		InkWarn_While_Argument_Require();
		return new Ink_NullObject();
	}

	cond = argv[0];
	block = argv[1];
	if (cond->type != INK_FUNCTION) {
		InkWarn_Require_Lazy_Expression();
		return new Ink_NullObject();
	} else if (block->type != INK_FUNCTION) {
		InkWarn_While_Block_Require();
		return new Ink_NullObject();
	}

	ret = new Ink_NullObject();
	while (isTrue(cond->call(context))) {
		ret = block->call(context);
		if (CGC_if_return) {
			return ret;
		}
	}

	return ret;
}

Ink_Object *Ink_ArrayConstructor(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_ContextChain *local = context->getLocal();
	Ink_Object *ret;

	if (argc) {
		if (argv[0]->type == INK_INTEGER && argc == 1) {
			ret = new Ink_Array(Ink_ArrayValue(as<Ink_Integer>(argv[0])->value, NULL));
		} else {
			Ink_ArrayValue val = Ink_ArrayValue();
			unsigned int i;
			for (i = 0; i < argc; i++) {
				val.push_back(new Ink_HashTable("", argv[i]));
			}
			ret = new Ink_Array(val);
		}
	} else {
		ret = new Ink_Array();
	}

	local->context->setSlot("this", ret);

	return ret;
}

Ink_Object *InkNative_Object_New(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv);
void Ink_GlobalMethodInit(Ink_ContextChain *context)
{
	context->context->setSlot("if", new Ink_FunctionObject(Ink_IfExpression, true));
	context->context->setSlot("while", new Ink_FunctionObject(Ink_WhileExpression, true));

	Ink_Object *array_cons = new Ink_FunctionObject(Ink_ArrayConstructor);
	context->context->setSlot("Array", array_cons);
	array_cons->setSlot("new", new Ink_FunctionObject(InkNative_Object_New));

	context->context->setSlot("undefined", new Ink_Undefined());
	context->context->setSlot("null", new Ink_NullObject());
}

int integer_native_method_table_count = 12;
InkNative_MethodTable integer_native_method_table[] = {
	{"+", new Ink_FunctionObject(InkNative_Integer_Add)},
	{"-", new Ink_FunctionObject(InkNative_Integer_Sub)},
	{"*", new Ink_FunctionObject(InkNative_Integer_Mul)},
	{"/", new Ink_FunctionObject(InkNative_Integer_Div)},
	{"==", new Ink_FunctionObject(InkNative_Integer_Equal)},
	{"!=", new Ink_FunctionObject(InkNative_Integer_NotEqual)},
	{">", new Ink_FunctionObject(InkNative_Integer_Greater)},
	{"<", new Ink_FunctionObject(InkNative_Integer_Less)},
	{">=", new Ink_FunctionObject(InkNative_Integer_GreaterOrEqual)},
	{"<=", new Ink_FunctionObject(InkNative_Integer_LessOrEqual)},
	{"+u", new Ink_FunctionObject(InkNative_Integer_Add_Unary)},
	{"-u", new Ink_FunctionObject(InkNative_Integer_Sub_Unary)}
};

int string_native_method_table_count = 2;
InkNative_MethodTable string_native_method_table[] = {
	{"+", new Ink_FunctionObject(InkNative_String_Add)},
	{"[]", new Ink_FunctionObject(InkNative_String_Index)}
};

int object_native_method_table_count = 6;
InkNative_MethodTable object_native_method_table[] = {
	{"->", new Ink_FunctionObject(InkNative_Object_Bond)},
	{"!!", new Ink_FunctionObject(InkNative_Object_Debond)},
	{"!", new Ink_FunctionObject(InkNative_Object_Not)},
	{"[]", new Ink_FunctionObject(InkNative_Object_Index)},
	{"new", new Ink_FunctionObject(InkNative_Object_New)},
	{"clone", new Ink_FunctionObject(InkNative_Object_Clone)}
};

int array_native_method_table_count = 3;
InkNative_MethodTable array_native_method_table[] = {
	{"[]", new Ink_FunctionObject(InkNative_Array_Index)},
	{"push", new Ink_FunctionObject(InkNative_Array_Push)},
	{"size", new Ink_FunctionObject(InkNative_Array_Size)}
};

int function_native_method_table_count = 1;
InkNative_MethodTable function_native_method_table[] = {
	{"<<", new Ink_FunctionObject(InkNative_Function_Insert)}
};