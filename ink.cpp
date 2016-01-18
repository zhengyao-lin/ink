#include <stdlib.h>
#include <typeinfo>
#include "core/hash.h"
#include "core/object.h"
#include "core/expression.h"
#include "core/debug.h"
#include "core/package/load.h"
#include "core/gc/collect.h"
#include "core/thread/thread.h"
#include "core/coroutine/coroutine.h"
#include "core/package/load.h"
#include "core/native/native.h"
#include "core/interface/engine.h"
#include "core/interface/setting.h"
#include "includes/universal.h"

using namespace std;

int main(int argc, char **argv)
{
	InkActor_initActorMap();
	Ink_initNativeExpression();
	// Ink_GlobalConstantInit();
	pthread_t root_actor = pthread_self();

	Ink_InterpreteEngine *engine = new Ink_InterpreteEngine();
	string *tmp_name = new string("root");
	InkActor_addActor(*tmp_name, engine, root_actor, tmp_name);
	engine->startParse(Ink_InputSetting::parseArg(argc, argv));

	engine->execute();

	InkActor_setDeadActor(engine);
	InkActor_joinAllActor(engine);

	delete engine;
	cleanAll(engine);
	closeAllHandler();
	Ink_cleanNativeExpression();

	return 0;
}
