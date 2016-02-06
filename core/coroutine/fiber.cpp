#include "coroutine.h"

#ifdef INK_PLATFORM_WIN32

namespace ink {

void InkCoro_Scheduler::destroy(InkCoro_Routine *co)
{
	InkCoro_RoutinePool::iterator pool_iter;

	if ((pool_iter = std::find(pool.begin(), pool.end(), co))
		!= pool.end()) {
		(*pool_iter)->state = INKCO_DEAD;
	}

	return;
}
void InkCoro_Scheduler::wrapper(LPVOID arg)
{
	InkCoro_Scheduler_wrapper_arg *tmp = (InkCoro_Scheduler_wrapper_arg *)arg;
	tmp->co->func(tmp->co->arg);
	tmp->sched->destroy(tmp->co);
	SwitchToFiber(tmp->sched->main_fib);
	return;
}
int InkCoro_Scheduler::create(InkCoro_Function fp, void *arg)
{
	InkCoro_Routine *co = new InkCoro_Routine();

	co->fib = CreateFiber(0, (LPFIBER_START_ROUTINE)wrapper,
						  co->tmp_arg = new InkCoro_Scheduler_wrapper_arg(this, co));
	co->func = fp;
	co->arg = arg;

	pool.push_back(co);

	return 0;
}
bool InkCoro_Scheduler::switchRoutine()
{
	InkCoro_RoutinePool::size_type i;

	i = current + 1;
	while (i < pool.size()) {
		if (pool[i] && pool[i]->state != INKCO_DEAD) {
			current = i;
			return true;
		}
		i++;
	}

	i = 0;
	while (i < current) {
		if (pool[i] && pool[i]->state != INKCO_DEAD) {
			current = i;
			return true;
		}
		i++;
	}

	return false;
}
void InkCoro_Scheduler::schedule()
{
	do {
		pool[current]->state = INKCO_RUNNING;

		SwitchToFiber(pool[current]->fib);

		if (pool[current]->state == INKCO_DEAD) {
			DeleteFiber(pool[current]->fib);
			delete pool[current];
			pool[current] = NULL;
		}
	} while (switchRoutine());
	return;
}
void InkCoro_Scheduler::yield()
{
	SwitchToFiber(main_fib);
}

void Ink_initCoroutine()
{
	ConvertThreadToFiberEx(NULL, FIBER_FLAG_FLOAT_SWITCH);
	return;
}

}

#endif
