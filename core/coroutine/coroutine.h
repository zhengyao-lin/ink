#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <vector>
#include "../general.h"
#include "../../includes/universal.h"

class Ink_Object;
class Ink_FunctionObject;

class Ink_CoCall {
public:
	Ink_FunctionObject *func;
	Ink_ArgcType argc;
	Ink_Object **argv;

	Ink_CoCall(Ink_FunctionObject *func,
			   Ink_ArgcType argc, Ink_Object **argv)
	: func(func), argc(argc), argv(argv)
	{ }
};
typedef std::vector<Ink_CoCall> Ink_CoCallList;

#endif
