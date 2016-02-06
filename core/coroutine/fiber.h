#ifndef _FIBER_H_
#define _FIBER_H_

#include "../../includes/universal.h"

#ifdef INK_PLATFORM_WIN32

namespace ink {

typedef void (*InkCoro_Function)(void*);

enum InkCoro_State {
	INKCO_READY,
	INKCO_RUNNING,
	INKCO_DEAD
};

class InkCoro_Scheduler;
struct InkCoro_Routine;

class InkCoro_Scheduler_wrapper_arg {
public:
	InkCoro_Scheduler *sched;
	InkCoro_Routine *co;

	InkCoro_Scheduler_wrapper_arg(InkCoro_Scheduler *sched,
								  InkCoro_Routine *co)
	: sched(sched), co(co)
	{ }
};

struct InkCoro_Routine {
	LPVOID fib;

	void *arg;
	InkCoro_Function func;
	InkCoro_State state;

	InkCoro_Scheduler_wrapper_arg *tmp_arg;

	InkCoro_Routine()
	{
		fib = 0;
		arg = NULL;
		func = NULL;
		state = INKCO_READY;
		tmp_arg = NULL;
	}

	~InkCoro_Routine()
	{
		if (tmp_arg) {
			delete tmp_arg;
		}
	}
};

typedef std::vector<InkCoro_Routine *> InkCoro_RoutinePool;

class InkCoro_Scheduler {
	LPVOID main_fib;
	bool is_nested;

	InkCoro_RoutinePool pool;
	InkCoro_RoutinePool::size_type current;
public:

	InkCoro_Scheduler()
	{
		// memset(&env, 0, sizeof(ucontext_t));
		main_fib = GetCurrentFiber();
		pool = InkCoro_RoutinePool();
		current = 0;
		return;
	}

	void destroy(InkCoro_Routine *co);
	static void wrapper(LPVOID arg);
	int create(InkCoro_Function fp, void *arg);
	bool switchRoutine();
	void schedule();
	void yield();
};

}

#endif

#endif
