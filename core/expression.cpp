#include <stdio.h>
#include <string.h>
#include "expression.h"
#include "coroutine/coroutine.h"

InterruptSignal CGC_interrupt_signal = INTER_NONE;

Ink_Expression *Ink_NumericConstant::parse(string code)
{
	unsigned long val = 0, flag = 1;
	double fval = 0.0;

	if (code[0] == '-') {
		flag = -1;
		code = code.substr(1);
	}

	if (sscanf(code.c_str(), "%lf", &fval) > 0)
		return new Ink_NumericConstant(fval * flag);
	if (sscanf(code.c_str(), "0x%lx", &val) > 0
		|| sscanf(code.c_str(), "0X%lX", &val) > 0
		|| sscanf(code.c_str(), "0%lo", &val) > 0
		|| sscanf(code.c_str(), "%lu", &val) > 0)
		return new Ink_NumericConstant(val * flag);

	return NULL;
}

Ink_Object *Ink_FunctionExpression::eval(Ink_ContextChain *context_chain)
{
	int line_num_back;
	SET_LINE_NUM;
	RESTORE_LINE_NUM;
	return new Ink_FunctionObject(param, exp_list, context_chain->copyContextChain(), is_inline, is_generator);
}