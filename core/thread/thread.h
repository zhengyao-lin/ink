#ifndef _THREAD_H_
#define _THREAD_H_

#include "../../includes/universal.h"

#if defined(INK_PLATFORM_LINUX)
	#include <map>
	#include <vector>
	#include <stdio.h>
	#include <malloc.h>
	#include <pthread.h>

	#define getThreadID_raw() (pthread_self())

	class MutexLock {
	public:
		pthread_mutex_t _lock;

		void init()
		{
			pthread_mutex_init(&_lock, NULL);
		}

		void lock()
		{
			pthread_mutex_lock(&_lock);
		}

		void unlock()
		{
			pthread_mutex_unlock(&_lock);
		}
	};

	class Ink_Expression;
	class Ink_ContextChain;

	class EvalArgument {
	public:
		Ink_Expression *exp;
		Ink_ContextChain *context;

		EvalArgument(Ink_Expression *exp, Ink_ContextChain *context)
		: exp(exp), context(context)
		{ }
	};

	typedef std::map<int, int> ThreadIDMap;
	typedef std::vector<pthread_t *> ThreadPool;

	int initThread();
	int getThreadID();
	int registerThread();
	void addThread(pthread_t *thread);
	void joinAllThread();
#endif

#endif