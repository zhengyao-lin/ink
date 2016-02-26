#ifndef _ACTOR_H_
#define _ACTOR_H_

#include <string>
#include <queue>
#include <map>
#include "thread.h"
#include "../general.h"
#include "../exception.h"

namespace ink {

class Ink_InterpreteEngine;

class Ink_ActorHandler {
public:
	Ink_InterpreteEngine *engine;
	pthread_t thread_handle;
	bool finished;
	std::string *name_p;
	bool is_root;

	Ink_ActorHandler()
	: engine(NULL), thread_handle(0), finished(false), name_p(NULL), is_root(false)
	{ }

	Ink_ActorHandler(Ink_InterpreteEngine *engine, pthread_t thread_handle, std::string *name_p, bool is_root = false)
	: engine(engine), thread_handle(thread_handle), finished(false), name_p(name_p), is_root(is_root)
	{ }

	~Ink_ActorHandler()
	{
		if (name_p) delete name_p;
	}
};

typedef std::map<std::string, Ink_ActorHandler *> Ink_ActorMap;
typedef Ink_ActorMap::size_type Ink_ActorCountType;
// typedef std::string *Ink_ActorMessage;

class Ink_ActorMessage {
public:
	std::string *msg;
	std::string *sender;
	Ink_ExceptionRaw *ex;

	Ink_ActorMessage(std::string *msg, std::string *sender, Ink_ExceptionRaw *ex = NULL)
	: msg(msg), sender(sender), ex(ex)
	{ }
	
	~Ink_ActorMessage()
	{
		delete msg;
		delete sender;
		delete ex;
	}
};

typedef std::queue<Ink_ActorMessage *> Ink_ActorMessageQueue;
typedef std::vector<std::string> Ink_ActorWatcherList;

void InkActor_lockThreadCreateLock();
void InkActor_unlockThreadCreateLock();
void InkActor_lockActorLock();
void InkActor_unlockActorLock();
void InkActor_initActorMap();
bool InkActor_addActor(std::string name, Ink_InterpreteEngine *engine, pthread_t handle, std::string *name_p, bool is_root = false);
bool InkActor_setRootEngine(Ink_InterpreteEngine *engine);
void InkActor_setDeadActor(Ink_InterpreteEngine *engine);
Ink_InterpreteEngine *InkActor_getActor(std::string name);
Ink_InterpreteEngine *InkActor_getActor_nolock(std::string name);
void InkActor_joinAllActor(Ink_InterpreteEngine *self_engine, Ink_InterpreteEngine *except = NULL);
Ink_ActorCountType InkActor_getActorCount();
std::string *InkActor_getActorName(Ink_InterpreteEngine *engine);
std::string *InkActor_getActorName_nolock(Ink_InterpreteEngine *engine);
bool InkActor_isRootActor(Ink_InterpreteEngine *engine);

}

#endif
