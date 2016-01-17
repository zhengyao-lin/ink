#include <string>
#include "actor.h"
#include "interface/engine.h"
#include "thread/thread.h"

using namespace std;

pthread_mutex_t ink_global_actor_map_lock;
pthread_mutex_t ink_actor_pthread_create_lock;
static Ink_ActorMap ink_global_actor_map;

void InkActor_initActorMap()
{
	pthread_mutex_init(&ink_global_actor_map_lock, NULL);
	pthread_mutex_init(&ink_actor_pthread_create_lock, NULL);
	ink_global_actor_map = Ink_ActorMap();

	return;
}

bool InkActor_addActor(string name, Ink_InterpreteEngine *engine, pthread_t handle, string *name_p)
{
	bool ret = true;
	pthread_mutex_lock(&ink_global_actor_map_lock);
	if (ink_global_actor_map.find(name) != ink_global_actor_map.end()) {
		InkWarn_Actor_Conflict(engine, name.c_str());
		ret = false;
	} else {
		ink_global_actor_map[name] = new Ink_ActorHandle(engine, handle, name_p);
	}
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return ret;
}

void InkActor_setDeadActor(Ink_InterpreteEngine *engine)
{
	Ink_ActorMap::iterator actor_it;
	pthread_mutex_lock(&ink_global_actor_map_lock);
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
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return;
}

void InkActor_joinAllActor(Ink_InterpreteEngine *self_engine, Ink_InterpreteEngine *except)
{
	Ink_ActorMap::iterator actor_it;
	bool finished;

AGAIN:
	pthread_mutex_lock(&ink_global_actor_map_lock);
	for (actor_it = ink_global_actor_map.begin(), finished = true;
		 actor_it != ink_global_actor_map.end();) {
		// printf("waiting %s\n", actor_it->first.c_str());
		if (actor_it->second && actor_it->second->engine != self_engine
			&& (!except || actor_it->second->engine != except)) {
			if (actor_it->second->finished) {
				delete actor_it->second;
				ink_global_actor_map.erase(actor_it++);
				continue;
			} else {
				finished = false;
			}
		}
		actor_it++;
	}
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	if (!finished) goto AGAIN;

	return;
}

Ink_InterpreteEngine *InkActor_getActor(string name)
{
	Ink_InterpreteEngine *ret = NULL;
	pthread_mutex_lock(&ink_global_actor_map_lock);
	if (ink_global_actor_map.find(name) != ink_global_actor_map.end()) {
		ret = ink_global_actor_map[name]->engine;
	}
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return ret;
}

unsigned int InkActor_getActorCount()
{
	unsigned int ret;
	Ink_ActorMap::iterator actor_it;

	pthread_mutex_lock(&ink_global_actor_map_lock);

	for (actor_it = ink_global_actor_map.begin(), ret = 0;
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		if (!actor_it->second->finished) ret++;
	}

	pthread_mutex_unlock(&ink_global_actor_map_lock);

	return ret;
}

string *InkActor_getActorName(Ink_InterpreteEngine *engine)
{
	string *ret = NULL;
	Ink_ActorMap::iterator actor_it;

	pthread_mutex_lock(&ink_global_actor_map_lock);

	for (actor_it = ink_global_actor_map.begin(), ret = 0;
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		if (actor_it->second->engine == engine) {
			ret = new string(actor_it->first);
		}
	}

	pthread_mutex_unlock(&ink_global_actor_map_lock);

	return ret;
}