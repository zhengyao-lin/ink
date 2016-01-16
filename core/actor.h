#ifndef _ACTOR_H_
#define _ACTOR_H_

#include <string>
#include <queue>
#include <map>
#include "thread/thread.h"

class Ink_InterpreteEngine;
typedef std::pair<Ink_InterpreteEngine*, pthread_t> Ink_ActorHandle;
typedef std::map<std::string, Ink_ActorHandle> Ink_ActorMap;
typedef std::string *Ink_ActorMessage;
typedef std::queue<Ink_ActorMessage> Ink_ActorMessageQueue;

void InkActor_initActorMap();
void InkActor_addActor(std::string name, Ink_InterpreteEngine *engine, pthread_t handle);
void InkActor_setDeadActor(Ink_InterpreteEngine *engine);
Ink_InterpreteEngine *InkActor_getActor(std::string name);
void InkActor_joinAllActor();

#endif