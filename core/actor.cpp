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

void InkActor_addActor(string name, Ink_InterpreteEngine *engine, pthread_t handle, string *name_p)
{
	pthread_mutex_lock(&ink_global_actor_map_lock);
	ink_global_actor_map[name] = new Ink_ActorHandle(engine, handle, name_p);
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return;
}

void InkActor_setDeadActor(Ink_InterpreteEngine *engine)
{
	Ink_ActorMap::iterator actor_it;
	pthread_mutex_lock(&ink_global_actor_map_lock);
	for (actor_it = ink_global_actor_map.begin();
		 actor_it != ink_global_actor_map.end(); actor_it++) {
		if (actor_it->second->engine == engine) {
			actor_it->second->engine = NULL;
			printf("hello?\n");
			// pthread_mutex_unlock(&actor_it->second->thread_lock);
			actor_it->second->finished = true;
			break;
		}
	}
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return;
}

void InkActor_joinAllActor(Ink_InterpreteEngine *self_engine)
{
	Ink_ActorMap::iterator actor_it;
	bool finished;

AGAIN:
	for (actor_it = ink_global_actor_map.begin(), finished = true;
		 actor_it != ink_global_actor_map.end();) {
		//printf("waiting %d\n", ink_global_actor_map.size());
		if (actor_it->second->engine != self_engine) {
			if (actor_it->second->finished) {
				pthread_mutex_lock(&ink_global_actor_map_lock);
				delete actor_it->second;
				ink_global_actor_map.erase(actor_it++);
				pthread_mutex_unlock(&ink_global_actor_map_lock);
				continue;
			} else {
				finished = false;
			}
		}
		actor_it++;
	}
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
	pthread_mutex_lock(&ink_global_actor_map_lock);
	ret = ink_global_actor_map.size();
	pthread_mutex_unlock(&ink_global_actor_map_lock);
	return ret;
}