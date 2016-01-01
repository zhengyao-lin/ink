#include <vector>
#include "general.h"
#include "object.h"
#include "expression.h"
#include "../includes/switches.h"
using namespace std;

vector<string *> string_pool;
Ink_ExpressionList native_exp_list = Ink_ExpressionList();

string *StrPool_addStr(const char *str)
{
	string *tmp;
	string_pool.push_back(tmp = new string(str ? str : ""));
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

void cleanAll()
{
	unsigned int i;
	for (i = 0; i < native_exp_list.size(); i++) {
		delete native_exp_list[i];
	}

	// remove(INK_TMP_PATH);
	removeDir(INK_TMP_PATH);
}

Ink_Argument::~Ink_Argument()
{
	if (arg)
		delete arg;
	if (is_expand)
		delete expandee;
}