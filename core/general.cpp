#include <vector>
#include "general.h"
#include "object.h"
#include "expression.h"
#include "coroutine/coroutine.h"
#include "thread/actor.h"
#include "interface/engine.h"
#include "package/load.h"
#include "../includes/switches.h"

namespace ink {

using namespace std;

void Ink_initEnv()
{
	Ink_loadAllModules();
	InkActor_initActorMap();
	DBG_initSignalProc();
	Ink_initNativeExpression();
	Ink_initCoroutine();
	return;
}

void Ink_removeTmpDir()
{
	if (isDirExist(INK_TMP_PATH))
		removeDir(INK_TMP_PATH);
}

void Ink_disposeEnv()
{
	Ink_removeTmpDir();
	Ink_disposeModules();
	Ink_cleanNativeExpression();
	return;
}

Ink_Argument::~Ink_Argument()
{
	if (arg)
		delete arg;
	if (is_expand)
		delete expandee;
}

Ink_SInt64 getInt(Ink_Object *obj)
{
	return as<Ink_Numeric>(obj)->getValue().toInt();
}

double getFloat(Ink_Object *obj)
{
	return as<Ink_Numeric>(obj)->getValue().toFloat();
}

bool getBool(Ink_Object *obj)
{
	return as<Ink_Numeric>(obj)->getValue().toBool();
}

}
