#include "thread.h"

#if defined(INK_PLATFORM_LINUX)
	MutexLock thread_lock;
	int current_id = 0;
	ThreadIDMap thread_id_map;
	ThreadPool thread_pool;

	int initThread()
	{
		thread_lock.init();
		return 0;
	}

	int getThreadID()
	{
		int id;

		thread_lock.lock();
		id = thread_id_map[getThreadID_raw()];
		thread_lock.unlock();

		return id;
	}

	int registerThread()
	{
		int id;

		thread_lock.lock();
		id = thread_id_map[getThreadID_raw()] = current_id++;
		thread_lock.unlock();

		return id;
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
#endif