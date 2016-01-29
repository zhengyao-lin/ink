#include <vector>
#include "general.h"
#include "object.h"
#include "expression.h"
#include "interface/engine.h"
#include "package/load.h"
#include "../includes/switches.h"

namespace ink {

using namespace std;

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

}
