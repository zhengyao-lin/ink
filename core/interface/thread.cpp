#include "engine.h"

#define CURRENT_LAYER (thread_id_map_stack.size() - 1)

namespace ink {

int Ink_InterpreteEngine::initThread()
{
	//thread_lock.init();
	pthread_mutex_init(&thread_pool_lock, NULL);
	return 0;
}

ThreadID Ink_InterpreteEngine::getThreadID()
{
	ThreadID id;

	pthread_mutex_lock(&thread_pool_lock);
	id = thread_id_map_stack[CURRENT_LAYER][getThreadID_raw()];
	pthread_mutex_unlock(&thread_pool_lock);

	return id;
}

ThreadID Ink_InterpreteEngine::registerThread(ThreadID id)
{
	pthread_mutex_lock(&thread_pool_lock);
	thread_id_map_stack[CURRENT_LAYER][getThreadID_raw()] = id;
	pthread_mutex_unlock(&thread_pool_lock);

	return id;
}

void Ink_InterpreteEngine::addLayer()
{
	pthread_mutex_lock(&thread_pool_lock);
	thread_id_map_stack.push_back(ThreadIDMap());
	pthread_mutex_unlock(&thread_pool_lock);
}

void Ink_InterpreteEngine::removeLayer()
{
	pthread_mutex_lock(&thread_pool_lock);
	thread_id_map_stack.pop_back();
	pthread_mutex_unlock(&thread_pool_lock);
}

ThreadLayerType Ink_InterpreteEngine::getCurrentLayer()
{
	pthread_mutex_lock(&thread_pool_lock);
	ThreadLayerType ret = CURRENT_LAYER;
	pthread_mutex_unlock(&thread_pool_lock);
	return ret;
}

void Ink_InterpreteEngine::addThread(pthread_t *thread)
{
	pthread_mutex_lock(&thread_pool_lock);
	thread_pool.push_back(thread);
	pthread_mutex_unlock(&thread_pool_lock);

	return;
}

void Ink_InterpreteEngine::joinAllThread()
{
	pthread_t *thd;
	ThreadPool::size_type i;
	for (i = 0; i < thread_pool.size(); i++) {
		pthread_mutex_lock(&thread_pool_lock);
		thd = thread_pool[i];
		pthread_mutex_unlock(&thread_pool_lock);

		pthread_join(*thd, NULL);
		free(thd);
	}
	return;
}

}
