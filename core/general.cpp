#include <vector>
#include "general.h"
#include "object.h"
#include "expression.h"
#include "interface/engine.h"
#include "package/load.h"
#include "../includes/switches.h"

using namespace std;

/*
static vector<string *> string_pool;

string *StrPool_addStr(const char *str)
{
	string *tmp;
	string_pool.push_back(tmp = new string(str ? str : ""));
	return tmp;
}

string *StrPool_addStr(string *str)
{
	string_pool.push_back(str);
	return str;
}

void StrPool_dispose()
{
	unsigned int i;
	for (i = 0; i < string_pool.size(); i++) {
		delete string_pool[i];
	}
	string_pool = vector<string *>();
	return;
}
*/

Ink_Object *trapSignal(Ink_InterpreteEngine *engine)
{
	Ink_Object *tmp = engine->CGC_interrupt_value;
	engine->CGC_interrupt_signal = INTER_NONE;
	engine->CGC_interrupt_value = NULL;
	return tmp;
}

void cleanAll(Ink_InterpreteEngine *engine)
{
	if (isDirExist(INK_TMP_PATH))
		removeDir(INK_TMP_PATH);
}

Ink_Argument::~Ink_Argument()
{
	if (arg)
		delete arg;
	if (is_expand)
		delete expandee;
}
