#ifndef _ACTOR_H_
#define _ACTOR_H_

#include <string>
#include <queue>
#include <map>
#include "general.h"
#include "thread/thread.h"

class Ink_InterpreteEngine;

class Ink_ActorHandle {
public:
	Ink_InterpreteEngine *engine;
	pthread_t thread_handle;
	bool finished;
	std::string *name_p;

	Ink_ActorHandle()
	: engine(NULL), thread_handle(0), finished(false), name_p(NULL)
	{ }

	Ink_ActorHandle(Ink_InterpreteEngine *engine, pthread_t thread_handle, std::string *name_p)
	: engine(engine), thread_handle(thread_handle), finished(false), name_p(name_p)
	{ }

	~Ink_ActorHandle()
	{
		if (name_p) delete name_p;
	}
};

typedef std::map<std::string, Ink_ActorHandle *> Ink_ActorMap;
typedef std::string *Ink_ActorMessage;
typedef std::queue<Ink_ActorMessage> Ink_ActorMessageQueue;

void InkActor_initActorMap();
void InkActor_addActor(std::string name, Ink_InterpreteEngine *engine, pthread_t handle, std::string *name_p);
void InkActor_setDeadActor(Ink_InterpreteEngine *engine);
Ink_InterpreteEngine *InkActor_getActor(std::string name);
void InkActor_joinAllActor(Ink_InterpreteEngine *self_engine);
unsigned int InkActor_getActorCount();

#endif