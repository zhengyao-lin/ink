#include <stdio.h>
#include <string.h>
#include "expression.h"

bool CGC_if_return = false;

Ink_Expression *Ink_IntegerConstant::parse(string *str, bool if_delete)
{
	int val = 0, flag = 1;
	string code = string(str->c_str());

	if (code[0] == '-') {
		flag = -1;
		code = code.substr(1);
	}

	if (sscanf(code.c_str(), "0x%x", &val) > 0
		|| sscanf(code.c_str(), "0X%X", &val) > 0
		|| sscanf(code.c_str(), "0%o", &val) > 0
		|| sscanf(code.c_str(), "%d", &val) > 0)
		return new Ink_IntegerConstant(val * flag);

	if (if_delete) {
		delete str;
	}

	return NULL;
}

Ink_Object *Ink_FunctionExpression::eval(Ink_ContextChain *context_chain)
{
	// TODO: Copy context chain
	return new Ink_FunctionObject(param, exp_list, context_chain->copyContextChain());
}