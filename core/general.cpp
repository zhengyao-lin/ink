#include <vector>
#include "general.h"
#include "object.h"
#include "expression.h"
#include "../includes/switches.h"
using namespace std;

static vector<string *> string_pool;
Ink_ExpressionList native_exp_list = Ink_ExpressionList();
char *tmp_prog_path = NULL;

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

void cleanAll()
{
	unsigned int i;
	const char *tmp;
	for (i = 0; i < native_exp_list.size(); i++) {
		delete native_exp_list[i];
	}

	// remove(INK_TMP_PATH);
	StrPool_dispose();

	if (isDirExist(tmp = string(INK_TMP_PATH).c_str()))
		removeDir(tmp);
	if (tmp_prog_path)
		free(tmp_prog_path);
}

Ink_Argument::~Ink_Argument()
{
	if (arg)
		delete arg;
	if (is_expand)
		delete expandee;
}