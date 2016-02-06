#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <vector>
#include <algorithm>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../general.h"
#include "../../includes/universal.h"

#define INKCO_STACK_SIZE (1024 * 1024 * 10)

namespace ink {

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

void Ink_initCoroutine();

}

#ifdef INK_PLATFORM_WIN32

#include "fiber.h"

#else

#include <ucontext.h>

namespace ink {

typedef void (*InkCoro_Function)(void*);

enum InkCoro_State {
	INKCO_READY,
	INKCO_RUNNING,
	INKCO_DEAD
};

struct InkCoro_Routine {
	ucontext_t env;
	void *arg;
	InkCoro_Function func;
	InkCoro_State state;

	InkCoro_Routine()
	{
		arg = NULL;
		func = NULL;
		state = INKCO_READY;
	}
};

typedef std::vector<InkCoro_Routine *> InkCoro_RoutinePool;

class InkCoro_Scheduler {
	ucontext_t env;
	
	InkCoro_RoutinePool pool;
	InkCoro_RoutinePool::size_type current;
public:

	InkCoro_Scheduler()
	{
		// memset(&env, 0, sizeof(ucontext_t));
		pool = InkCoro_RoutinePool();
		current = 0;
		return;
	}

	void destroy(InkCoro_Routine *co);
	static void wrapper(uint32_t s_h, uint32_t s_l, uint32_t c_h, uint32_t c_l);
	int create(InkCoro_Function fp, void *arg);
	bool switchRoutine();
	void schedule();
	void yield();
};

}

#endif

#endif
