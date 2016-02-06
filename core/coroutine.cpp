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

void InkCoCall_primaryCall(InkCoCall_Argument *arg)
{
	arg->sync_call.func->call(arg->engine, arg->context, arg->sync_call.argc, arg->sync_call.argv);
	return;
}

Ink_Object *InkCoCall_call(Ink_InterpreteEngine *engine,
						   Ink_ContextChain *context,
						   Ink_CoCallList call_list)
{
	Ink_CoCallList::size_type i;
	InkCoCall_Argument *tmp;
	vector<InkCoCall_Argument *> dispose_list;
	vector<InkCoCall_Argument *>::size_type j;
	int err_code;

	InkCoro_Scheduler *sched;
	IGC_CollectEngine *coro_tmp_engine_back = engine->coro_tmp_engine;
	IGC_CollectEngine *gc_engine_backup = engine->getCurrentGC();

	engine->coro_tmp_engine = gc_engine_backup;
	sched = engine->newScheduler();

	for (i = 0; i < call_list.size(); i++) {
		tmp = new InkCoCall_Argument(engine, context, call_list[i], 0);
		if ((err_code = sched->create((InkCoro_Function)InkCoCall_primaryCall, tmp)) != 0) {
			InkWarn_Failed_Create_Coroutine(engine, err_code);
		}
		dispose_list.push_back(tmp);
	}

	sched->schedule();

	engine->popScheduler();
	for (j = 0; j < dispose_list.size(); j++) {
		delete dispose_list[j];
	}
	engine->setCurrentGC(gc_engine_backup);
	engine->coro_tmp_engine = coro_tmp_engine_back;

	return NULL_OBJ;
}

}
