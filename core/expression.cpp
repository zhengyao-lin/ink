#include <stdio.h>
#include <string.h>
#include "expression.h"

bool CGC_if_return = false;

Ink_Expression *Ink_IntegerConstant::parse(string code)
{
	unsigned int val = 0, flag = 1;

	if (code[0] == '-') {
		flag = -1;
		code = code.substr(1);
	}

	if (sscanf(code.c_str(), "0x%x", &val) > 0
		|| sscanf(code.c_str(), "0X%X", &val) > 0
		|| sscanf(code.c_str(), "0%o", &val) > 0
		|| sscanf(code.c_str(), "%u", &val) > 0)
		return new Ink_IntegerConstant(val * flag);

	return NULL;
}

Ink_Object *Ink_FunctionExpression::eval(Ink_ContextChain *context_chain)
{
	SET_LINE_NUM;
	return new Ink_FunctionObject(param, exp_list, context_chain->copyContextChain(), is_inline);
}