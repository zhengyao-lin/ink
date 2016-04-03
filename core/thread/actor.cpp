#include <string>
#include "actor.h"
#include "time.h"
#include "thread.h"
#include "../interface/engine.h"

namespace ink {

using namespace std;

static pthread_mutex_t ink_global_actor_map_lock;
static pthread_mutex_t ink_actor_pthread_create_lock;
static Ink_ActorMap ink_global_actor_map;

void InkActor_lockThreadCreateLock()
{
	pthread_mutex_lock(&ink_actor_pthread_create_lock);
	return;
}

void InkActor_unlockThreadCreateLock()
{
	pthread_mutex_unlock(&ink_actor_pthread_create_lock);
	return;
}

void InkActor_lockActorLock()
{
	pthread_mutex_lock(&ink_global_actor_map_lock);
	return;
}

void InkActor_unlockActorLock()
{
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return;
}

void InkActor_initActorMap()
{
	pthread_mutex_init(&ink_global_actor_map_lock, NULL);
	pthread_mutex_init(&ink_actor_pthread_create_lock, NULL);
	ink_global_actor_map = Ink_ActorMap();

	return;
}

bool InkActor_addActor(string name, Ink_InterpreteEngine *engine, pthread_t handle, string *name_p, bool is_root)
{
	bool ret = true;
	InkActor_lockActorLock();
	if (ink_global_actor_map.find(name) != ink_global_actor_map.end()) {
		InkWarn_Actor_Conflict(engine, name.c_str());
		ret = false;
	} else {
		ink_global_actor_map[name] = new Ink_ActorHandler(engine, handle, name_p, is_root);
	}
	InkActor_unlockActorLock();
	return ret;
}

bool InkActor_setRootEngine(Ink_InterpreteEngine *engine)
{
	string *tmp_name = new string(INK_ROOT_ACTOR_NAME);
	return InkActor_addActor(*tmp_name, engine, pthread_self(), tmp_name, true);
}

void InkActor_setDeadActor(Ink_InterpreteEngine *engine)
{
	Ink_ActorMap::iterator actor_it;
	InkActor_lockActorLock();
	for (actor_it = ink_global_actor_map.begin();
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		if (actor_it->second->engine == engine) {
			actor_it->second->engine = NULL;
			// pthread_mutex_unlock(&actor_it->second->thread_lock);
			actor_it->second->finished = true;
			// ink_global_actor_map.erase(actor_it);
			break;
		}
	}
	InkActor_unlockActorLock();
	return;
}

void InkActor_joinAllActor(Ink_InterpreteEngine *self_engine, Ink_InterpreteEngine *except)
{
	Ink_ActorMap::iterator actor_it;
	bool finished;

AGAIN:
	InkActor_lockActorLock();
	for (actor_it = ink_global_actor_map.begin(), finished = true;
		 actor_it != ink_global_actor_map.end();) {
		// printf("waiting %s\n", actor_it->first.c_str());
		if (actor_it->second && actor_it->second->engine != self_engine
			&& (!except || actor_it->second->engine != except)) {
			if (actor_it->second->finished) {
				// pthread_join(actor_it->second->thread_handle, NULL);
				delete actor_it->second;
				ink_global_actor_map.erase(actor_it++);
				continue;
			} else {
				finished = false;
			}
		}
		actor_it++;
	}
	InkActor_unlockActorLock();
	if (!finished) goto AGAIN;

	return;
}

Ink_InterpreteEngine *InkActor_getActor(string name)
{
	Ink_InterpreteEngine *ret = NULL;
	InkActor_lockActorLock();
	if (ink_global_actor_map.find(name) != ink_global_actor_map.end()) {
		ret = ink_global_actor_map[name]->engine;
	}
	InkActor_unlockActorLock();
	return ret;
}

Ink_InterpreteEngine *InkActor_getActor_nolock(string name)
{
	Ink_InterpreteEngine *ret = NULL;
	if (ink_global_actor_map.find(name) != ink_global_actor_map.end()) {
		ret = ink_global_actor_map[name]->engine;
	}
	return ret;
}

Ink_ActorCountType InkActor_getActorCount()
{
	Ink_ActorCountType ret;
	Ink_ActorMap::iterator actor_it;

	InkActor_lockActorLock();

	for (actor_it = ink_global_actor_map.begin(), ret = 0;
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		if (!actor_it->second->finished) ret++;
	}

	InkActor_unlockActorLock();

	return ret;
}

string *InkActor_getActorName(Ink_InterpreteEngine *engine)
{
	string *ret = NULL;
	Ink_ActorMap::iterator actor_it;

	InkActor_lockActorLock();

	for (actor_it = ink_global_actor_map.begin(), ret = 0;
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		if (actor_it->second->engine == engine) {
			ret = new string(actor_it->first);
		}
	}

	InkActor_unlockActorLock();

	return ret;
}

string *InkActor_getActorName_nolock(Ink_InterpreteEngine *engine)
{
	string *ret = NULL;
	Ink_ActorMap::iterator actor_it;

	for (actor_it = ink_global_actor_map.begin(), ret = 0;
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		if (actor_it->second->engine == engine) {
			ret = new string(actor_it->first);
		}
	}

	return ret;
}

bool InkActor_isRootActor(Ink_InterpreteEngine *engine)
{
	bool is_root = false;
	Ink_ActorMap::iterator actor_it;

	InkActor_lockActorLock();
	for (actor_it = ink_global_actor_map.begin();
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		if (actor_it->second->engine == engine) {
			is_root = actor_it->second->is_root;
		}
	}
	InkActor_unlockActorLock();

	return is_root;
}

void InkActor_printAllTrace()
{
	Ink_InterpreteEngine *engine;
	Ink_ActorMap::iterator actor_it;
	Ink_ActorMap::size_type i;

	InkActor_lockActorLock();
	for (actor_it = ink_global_actor_map.begin(), i = 1;
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		if ((engine = actor_it->second->engine) != NULL) {
			fprintf(stderr, "actor %ld: %s%s:\n", i, actor_it->first.c_str(),
					(actor_it->second->is_root ? "(root)" : ""));
			engine->printTrace(stderr, engine->getTrace(), "TRACE: ");
			fprintf(stderr, "\n");
			i++;
		}
	}
	InkActor_unlockActorLock();

	return;
}

}
