#include <vector>
#include "general.h"
#include "object.h"
#include "expression.h"
using namespace std;

vector<string *> string_pool;

string *StrPool_addStr(const char *str)
{
	string *tmp;
	string_pool.push_back(tmp = new string(str));
	return tmp;
}

void StrPool_dispose()
{
	unsigned int i;
	for (i = 0; i < string_pool.size(); i++) {
		delete string_pool[i];
	}
	return;
}

Ink_Argument::~Ink_Argument()
{
	if (arg)
		delete arg;
	if (is_expand)
		delete expandee;
}