#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <vector>
#include "../general.h"
#include "../../includes/universal.h"

class Ink_Object;
class Ink_Expression;
class Ink_FunctionObject;
class Ink_ContextChain;
class IGC_CollectEngine;
typedef std::vector<Ink_Expression *> Ink_ExpressionList;

class Ink_CoCall {
public:
	Ink_FunctionObject *func;
	unsigned int argc;
	Ink_Object **argv;

	Ink_CoCall(Ink_FunctionObject *func,
					unsigned int argc, Ink_Object **argv)
	: func(func), argc(argc), argv(argv)
	{ }
};
typedef std::vector<Ink_CoCall> Ink_CoCallList;

class Ink_CoroutineSlice {
public:
	Ink_FunctionObject *func;
	Ink_ContextChain *context;
	IGC_CollectEngine *engine;
	Ink_ExpressionList exp_list;
	unsigned int current_pc;

	Ink_CoroutineSlice(Ink_FunctionObject *func,
					   Ink_ContextChain *context,
					   IGC_CollectEngine *engine,
					   Ink_ExpressionList exp_list,
					   unsigned int current_pc)
	: func(func), context(context), engine(engine),
	  exp_list(exp_list), current_pc(current_pc)
	{ }
};
typedef std::vector<Ink_CoroutineSlice> Ink_Coroutine;
typedef std::vector<Ink_Coroutine> Ink_CoroutineList;

#endif