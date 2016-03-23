#include <stdlib.h>
#include <typeinfo>
#include "core/general.h"
#include "core/numeric.h"
#include "core/interface/engine.h"
#include "core/interface/setting.h"

using namespace ink;
using namespace std;

int main(int argc, char **argv)
{
	setlocale(LC_CTYPE, "");

	Ink_InputSetting setting = Ink_InputSetting::parseArg(argc, argv);
	Ink_InterpreteEngine *engine = NULL;

	if (!setting.if_run) {
		return 0;
	}

	Ink_initEnv();

	engine = new Ink_InterpreteEngine();
	InkActor_setRootEngine(engine);

	engine->applySetting(setting);
	engine->startParse(setting);
	engine->execute();

	InkActor_setDeadActor(engine);
	InkActor_joinAllActor(engine);

	delete engine;
	Ink_disposeEnv();

	return 0;
}
