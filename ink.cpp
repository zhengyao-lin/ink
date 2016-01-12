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
#include "core/interface/engine.h"
#include "core/interface/setting.h"

int main(int argc, char **argv)
{
	initThread();
	// registerThread();

	DBG_initTypeMapping();

	Ink_InterpreteEngine *engine = new Ink_InterpreteEngine();
	engine->startParse(Ink_InputSetting::parseArg(argc, argv));

	engine->execute();

	// joinAllThread();

	delete engine;
	cleanAll();
	DBG_disposeTypeMapping();
	closeAllHandler();

	return 0;
}