#include "thread.h"

/*
MutexLock thread_lock;
int current_id = 0;
ThreadIDMapStack thread_id_map_stack;
ThreadPool thread_pool;

#define CURRENT_LAYER (thread_id_map_stack.size() - 1)

int initThread()
{
	thread_lock.init();
	//thread_id_map_stack.push_back(ThreadIDMap());
	return 0;
}

ThreadID getThreadID()
{
	ThreadID id;

	thread_lock.lock();
	id = thread_id_map_stack[CURRENT_LAYER][getThreadID_raw()];
	thread_lock.unlock();

	return id;
}

ThreadID registerThread(int id)
{
	thread_lock.lock();
	thread_id_map_stack[CURRENT_LAYER][getThreadID_raw()] = id;
	thread_lock.unlock();

	return id;
}

void addLayer()
{
	thread_lock.lock();
	thread_id_map_stack.push_back(ThreadIDMap());
	thread_lock.unlock();
}

void removeLayer()
{
	thread_lock.lock();
	thread_id_map_stack.pop_back();
	thread_lock.unlock();
}

unsigned int getCurrentLayer()
{
	thread_lock.lock();
	unsigned int ret = CURRENT_LAYER;
	thread_lock.unlock();
	return ret;
}

void addThread(pthread_t *thread)
{
	thread_lock.lock();
	thread_pool.push_back(thread);
	thread_lock.unlock();

	return;
}

void joinAllThread()
{
	pthread_t *thd;
	unsigned int i;
	//thread_lock.lock();
	for (i = 0; i < thread_pool.size(); i++) {
		thread_lock.lock();
		thd = thread_pool[i];
		thread_lock.unlock();

		pthread_join(*thd, NULL);
		free(thd);
	}
	//thread_lock.unlock();
	return;
}
*/