#include <vector>
#include "../object.h"
#include "../context.h"
#include "../expression.h"

extern Ink_ExpressionList native_exp_list;

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
	Ink_Object *cond = argv[0];
	Ink_Object *ret;
	Ink_ParamList ret_param;
	Ink_ExpressionList ret_body;
	Ink_FunctionExpression *func_exp;

	// true block
	ret_param = Ink_ParamList();
	ret_param.push_back(new string("true_block"));

	ret_body = Ink_ExpressionList();

	if (isTrue(cond)) { // true
		ret_body.push_back(new Ink_CallExpression(new Ink_IdentifierExpression(new string("true_block")),
												  Ink_ExpressionList()));

		ret_body.push_back(new Ink_AssignmentExpression(
								new Ink_HashExpression(
									new Ink_IdentifierExpression(new string("ret")),
									new string("else")
								),
								getEmptyBlockFunction()
							));
	} else { // false
		Ink_ParamList ret_param2;
		Ink_ExpressionList ret_body2;

		ret_param2 = Ink_ParamList();
		ret_param2.push_back(new string("false_block"));

		ret_body2 = Ink_ExpressionList();
		ret_body2.push_back(new Ink_CallExpression(new Ink_IdentifierExpression(new string("false_block")),
												  Ink_ExpressionList()));

		ret_body.push_back(new Ink_AssignmentExpression(
								new Ink_HashExpression(
									new Ink_IdentifierExpression(new string("ret")),
									new string("else")
								),
								new Ink_FunctionExpression(ret_param2, ret_body2, true)
							));
	}
	ret_body.push_back(new Ink_IdentifierExpression(new string("ret")));

	func_exp = new Ink_FunctionExpression(ret_param, ret_body, true);
	native_exp_list.push_back(func_exp);

	ret = func_exp->eval(context);

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

	Ink_Object *array_cons = new Ink_FunctionObject(Ink_ArrayConstructor);
	context->context->setSlot("Array", array_cons);
	array_cons->setSlot("new", new Ink_FunctionObject(InkNative_Object_New));

	context->context->setSlot("undefined", new Ink_Undefined());
	context->context->setSlot("null", new Ink_NullObject());
}