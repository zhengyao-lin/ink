#include <stdio.h>
#include <string.h>
#include "expression.h"
#include "coroutine/coroutine.h"

/* interrupt signal */
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

Ink_Object *Ink_FunctionExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;
	RESTORE_LINE_NUM;
	return new Ink_FunctionObject(param, exp_list, context_chain->copyContextChain(), is_inline, is_generator);
}

inline Ink_ArgumentList expandArgument(Ink_Object *obj)
{
	Ink_ArgumentList ret = Ink_ArgumentList();
	Ink_ArrayValue arr_val;
	unsigned int i;

	if (!obj || obj->type != INK_ARRAY) {
		InkWarn_With_Attachment_Require();
		return ret;
	}

	arr_val = as<Ink_Array>(obj)->value;

	for (i = 0; i < arr_val.size(); i++) {
		ret.push_back(new Ink_Argument(new Ink_ShellExpression(arr_val[i]->getValue())));
	}
	return ret;
}

Ink_Object *Ink_CallExpression::eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
{
	int line_num_back;
	SET_LINE_NUM;

	unsigned int i;
	Ink_Object **argv = NULL;
	Ink_Object *ret_val;
	Ink_Object *func = callee->eval(context_chain);
	Ink_ParamList param_list = Ink_ParamList();
	Ink_ArgumentList dispose_list, tmp_arg_list, another_tmp_arg_list;

	if (func->type == INK_FUNCTION) {
		param_list = as<Ink_FunctionObject>(func)->param;
	}

	for (i = 0, tmp_arg_list = Ink_ArgumentList();
		 i < arg_list.size(); i++) {
		if (arg_list[i]->is_expand) {
			another_tmp_arg_list = expandArgument(arg_list[i]->expandee->eval(context_chain));
			dispose_list.insert(dispose_list.end(), another_tmp_arg_list.begin(), another_tmp_arg_list.end());
			tmp_arg_list.insert(tmp_arg_list.end(), another_tmp_arg_list.begin(), another_tmp_arg_list.end());
		} else {
			tmp_arg_list.push_back(arg_list[i]);
		}
	}

	if (tmp_arg_list.size()) {
		argv = (Ink_Object **)malloc(tmp_arg_list.size() * sizeof(Ink_Object *));
		for (i = 0; i < tmp_arg_list.size(); i++) {
			if (i < param_list.size() && param_list[i].is_ref) {
				Ink_ExpressionList exp_list = Ink_ExpressionList();
				exp_list.push_back(tmp_arg_list[i]->arg);
				argv[i] = new Ink_FunctionObject(Ink_ParamList(), exp_list,
												 context_chain->copyContextChain(),
												 true);
			} else {
				argv[i] = tmp_arg_list[i]->arg->eval(context_chain);
			}
		}
	}

	ret_val = func->call(context_chain, tmp_arg_list.size(), argv);

	free(argv);

	for (i = 0; i < dispose_list.size(); i++) {
		delete dispose_list[i];
	}

	RESTORE_LINE_NUM;
	return ret_val;
}