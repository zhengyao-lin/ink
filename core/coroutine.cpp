#include "object.h"
#include "gc/collect.h"
#include "interface/engine.h"
#include "coroutine/coroutine.h"
#include "thread/thread.h"

namespace ink {

class InkCoCall_Argument {
public:
	Ink_InterpreteEngine *engine;
	Ink_ContextChain *context;
	Ink_CoCall sync_call;
	ThreadID id;

	InkCoCall_Argument(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_CoCall sync_call, ThreadID id)
	: engine(engine), context(context), sync_call(sync_call), id(id)
	{ }
};

bool /* if found idling coroutine */
InkCoCall_switchCoroutine(Ink_InterpreteEngine *engine)
{
	ThreadID id = engine->ink_sync_call_current_thread + 1;
	while (id < engine->ink_sync_call_max_thread) {
		if (!engine->ink_sync_call_end_flag[id]) {
			engine->ink_sync_call_current_thread = id;
			return true;
		}
		id++;
	}
	id = 1;
	while (id < engine->ink_sync_call_current_thread) {
		if (!engine->ink_sync_call_end_flag[id]) {
			engine->ink_sync_call_current_thread = id;
			return true;
		}
		id++;
	}
	return false;
}

void *InkCoCall_primaryCall(void *arg)
{
	InkCoCall_Argument *tmp = (InkCoCall_Argument *)arg;
	ThreadID self_id = tmp->engine->registerThread(tmp->id);
	ThreadLayerType self_layer = tmp->engine->getCurrentLayer();
	// printf("***Coroutine created: id %d at layer %u\n", self_id, self_layer);

REWAIT:
	do {
		while (1) {
			pthread_mutex_lock(&tmp->engine->ink_sync_call_mutex);
			if (tmp->engine->ink_sync_call_current_thread == self_id) {
				pthread_mutex_unlock(&tmp->engine->ink_sync_call_mutex);
				break;
			}
			pthread_mutex_unlock(&tmp->engine->ink_sync_call_mutex);
		}
	} while (tmp->engine->getCurrentLayer() != self_layer);
	pthread_mutex_lock(&tmp->engine->ink_sync_call_mutex);
	if (tmp->engine->ink_sync_call_current_thread != self_id) {
		pthread_mutex_unlock(&tmp->engine->ink_sync_call_mutex);
		goto REWAIT;
	}
	pthread_mutex_unlock(&tmp->engine->ink_sync_call_mutex);

	Ink_Object *ret_val = tmp->sync_call.func->call(tmp->engine, tmp->context, tmp->sync_call.argc,
													tmp->sync_call.argv);

	pthread_mutex_lock(&tmp->engine->ink_sync_call_mutex);
	// removeThread();
	InkCoCall_switchCoroutine(tmp->engine);
	tmp->engine->ink_sync_call_end_flag[self_id] = true;
	pthread_mutex_unlock(&tmp->engine->ink_sync_call_mutex);

	// printf("***Coroutine ended: id %u at layer %u\n", self_id, self_layer);

	return ret_val;
}

Ink_Object *InkCoCall_call(Ink_InterpreteEngine *engine,
						   Ink_ContextChain *context,
						   Ink_CoCallList call_list)
{
	pthread_t *thread_pool;
	Ink_CoCallList::size_type i;
	ThreadID th_id;
	InkCoCall_Argument *tmp;
	vector<InkCoCall_Argument *> dispose_list;
	vector<InkCoCall_Argument *>::size_type j;
	Ink_Object *ret_val;
	Ink_ArrayValue arr_val;

	// printf("***Scheduler Started: %lu coroutines are going to be created\n", call_list.size());
	engine->addLayer();

	ThreadID ink_sync_call_max_thread_back = engine->ink_sync_call_max_thread;
	ThreadID ink_sync_call_current_thread_back = engine->ink_sync_call_current_thread;
	IGC_CollectEngine *ink_sync_call_tmp_engine_back = engine->ink_sync_call_tmp_engine;
	vector<bool> ink_sync_call_end_flag_back = engine->ink_sync_call_end_flag;

	IGC_CollectEngine *gc_engine_backup = engine->getCurrentGC();
	
	thread_pool = (pthread_t *)malloc(sizeof(pthread_t) * call_list.size());

	pthread_mutex_lock(&engine->ink_sync_call_mutex);
	engine->ink_sync_call_tmp_engine = gc_engine_backup;
	engine->ink_sync_call_current_thread = 0;
	engine->ink_sync_call_max_thread = call_list.size() + 1;
	engine->ink_sync_call_end_flag = vector<bool>(call_list.size() + 1, false);
	pthread_mutex_unlock(&engine->ink_sync_call_mutex);

	for (i = 0, th_id = 1; i < call_list.size(); i++, th_id++) {
		tmp = new InkCoCall_Argument(engine, context, call_list[i], th_id);
		pthread_create(&thread_pool[i], NULL, InkCoCall_primaryCall, tmp);
		dispose_list.push_back(tmp);
	}

	pthread_mutex_lock(&engine->ink_sync_call_mutex);
	engine->ink_sync_call_current_thread = 1;
	pthread_mutex_unlock(&engine->ink_sync_call_mutex);
	
	for (i = 0; i < call_list.size(); i++) {
		pthread_join(thread_pool[i], (void **)&ret_val);
		if (ret_val) {
			arr_val.push_back(new Ink_HashTable(ret_val));
		}
	}
	// printf("***Scheduler: All coroutine ended. Existing.\n");
	for (j = 0; j < dispose_list.size(); j++) {
		delete dispose_list[j];
	}
	free(thread_pool);
	engine->setCurrentGC(gc_engine_backup);

	pthread_mutex_lock(&engine->ink_sync_call_mutex);
	engine->ink_sync_call_tmp_engine = ink_sync_call_tmp_engine_back;
	engine->ink_sync_call_max_thread = ink_sync_call_max_thread_back;
	engine->ink_sync_call_current_thread = ink_sync_call_current_thread_back;
	engine->ink_sync_call_end_flag = ink_sync_call_end_flag_back;
	pthread_mutex_unlock(&engine->ink_sync_call_mutex);

	engine->removeLayer();

	return new Ink_Array(engine, arr_val);
}

}
