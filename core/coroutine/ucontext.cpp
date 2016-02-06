#include "coroutine.h"

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

void InkCoro_Scheduler::wrapper(uint32_t s_h, uint32_t s_l, uint32_t c_h, uint32_t c_l)
{
	InkCoro_Routine *co = (InkCoro_Routine *)(((uintptr_t)c_h << 32) | c_l);
	InkCoro_Scheduler *sched = (InkCoro_Scheduler *)(((uintptr_t)s_h << 32) | s_l);

	co->func(co->arg);

	sched->destroy(co);
}

int InkCoro_Scheduler::create(InkCoro_Function fp, void *arg)
{
	InkCoro_Routine *co = new InkCoro_Routine();
	int err_code;

	co->func = fp;
	co->arg = arg;
	co->state = INKCO_READY;

	if ((err_code = getcontext(&co->env)) < 0) {
		return err_code;
	}

	if ((err_code = posix_memalign(&co->env.uc_stack.ss_sp,
								   8, INKCO_STACK_SIZE)) != 0) {
		return err_code;
	}

	co->env.uc_stack.ss_size = INKCO_STACK_SIZE;
	co->env.uc_link = &env;

	uintptr_t ul = (uintptr_t)co;
	uintptr_t self = (uintptr_t)this;
	makecontext(&co->env, (void (*)())wrapper, 4, (uint32_t)(self >> 32), self, (uint32_t)(ul >> 32), ul);

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
		swapcontext(&env, &pool[current]->env);

		if (pool[current]->state == INKCO_DEAD) {
			free(pool[current]->env.uc_stack.ss_sp);
			delete pool[current];
			pool[current] = NULL;
		}
	} while (switchRoutine());
	return;
}

void InkCoro_Scheduler::yield()
{
	if (pool[current]) {
		swapcontext(&pool[current]->env, &env);
	}
}

void Ink_initCoroutine() { return; }

}
