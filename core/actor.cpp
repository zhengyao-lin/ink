#include <string>
#include "actor.h"
#include "interface/engine.h"
#include "thread/thread.h"

using namespace std;

static pthread_mutex_t ink_global_actor_map_lock;
static Ink_ActorMap ink_global_actor_map;

void InkActor_initActorMap()
{
	pthread_mutex_lock(&ink_global_actor_map_lock);
	ink_global_actor_map = Ink_ActorMap();
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return;
}

void InkActor_addActor(string name, Ink_InterpreteEngine *engine, pthread_t handle)
{
	pthread_mutex_lock(&ink_global_actor_map_lock);
	ink_global_actor_map[name] = Ink_ActorHandle(engine, handle);
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return;
}

void InkActor_setDeadActor(Ink_InterpreteEngine *engine)
{
	Ink_ActorMap::iterator actor_it;
	pthread_mutex_lock(&ink_global_actor_map_lock);
	for (actor_it = ink_global_actor_map.begin();
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		if (actor_it->second.first == engine) {
			actor_it->second.first = NULL;
		}
	}
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return;
}

void InkActor_joinAllActor()
{
	Ink_ActorMap::iterator actor_it;

	for (actor_it = ink_global_actor_map.begin();
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		pthread_join(actor_it->second.second, NULL);
	}

	return;
}

Ink_InterpreteEngine *InkActor_getActor(string name)
{
	Ink_InterpreteEngine *ret = NULL;
	pthread_mutex_lock(&ink_global_actor_map_lock);
	if (ink_global_actor_map.find(name) != ink_global_actor_map.end()) {
		ret = ink_global_actor_map[name].first;
	}
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return ret;
}