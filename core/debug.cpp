#include <string>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "object.h"
#include "general.h"
#include "type.h"
#include "context.h"

using namespace std;

int dbg_type_mapping_length = 0;
DBG_TypeMapping *dbg_type_mapping = NULL;
struct TempMapping {
	Ink_TypeTag tag;
	const char *name;
} dbg_fixed_type_mapping[] = 
{
	{ INK_NULL,			"null" },
	{ INK_UNDEFINED,	"undefined" },
	{ INK_OBJECT,		"object" },
	{ INK_NUMERIC,		"numeric" },
	{ INK_STRING,		"string" },
	{ INK_CONTEXT,		"context" },
	{ INK_FUNCTION,		"function" },
	{ INK_ARRAY,		"array" },
	{ INK_UNKNOWN,		"unknown" }
};

void DBG_initTypeMapping()
{
	int i;

	dbg_type_mapping_length = INK_LAST;
	dbg_type_mapping = (DBG_TypeMapping *)malloc(sizeof(DBG_TypeMapping) * dbg_type_mapping_length);

	for (i = 0; i < INK_LAST; i++) {
		dbg_type_mapping[i] = DBG_TypeMapping(i, dbg_fixed_type_mapping[i].name);
	}

	return;
}

void DBG_disposeTypeMapping()
{
	free(dbg_type_mapping);
	return;
}

int DBG_registerType(const char *name)
{
	// StrPool_addStr(
	int ret = dbg_type_mapping_length++;
	dbg_type_mapping = (DBG_TypeMapping *)realloc(dbg_type_mapping,
												  sizeof(DBG_TypeMapping) * dbg_type_mapping_length);
	dbg_type_mapping[ret] = DBG_TypeMapping(ret, StrPool_addStr(name)->c_str());
	return ret;
}

vector<Ink_Object *> traced_stack;

void printSlotInfo(FILE *fp, Ink_Object *obj, string prefix = "")
{
	Ink_HashTable *i;
	unsigned int j;
	Ink_Array *arr;
	string getter_setter_info = "";

	if (!obj) {
		fprintf(fp, "\n");
		return;
	}
	fprintf(fp, " {\n");
	for (i = obj->hash_table; i; i = i->next) {
		getter_setter_info = i->getter ?
							 string(" [ has getter") + (i->setter ? " and setter ]" : " ]") :
							 (i->setter ? " [ has setter ]" : "");

		fprintf(fp, "%s" DBG_TAB "\'%s\':%s ", prefix.c_str(),
				(i->key && strlen(i->key) ? i->key : "anonymous"), getter_setter_info.c_str());
		DBG_printDebugInfo(fp, i->getValue(), "", DBG_TAB + prefix);
	}

	if (obj->type == INK_ARRAY) {
		arr = as<Ink_Array>(obj);
		for (j = 0; j < arr->value.size(); j++) {
			fprintf(fp, "%s" DBG_TAB "[%d]: ", prefix.c_str(), j);
			if (arr->value[j]) {
				DBG_printDebugInfo(fp, arr->value[j]->getValue(), "", DBG_TAB + prefix);
			} else {
				fprintf(fp, "(no value)\n");
			}
		}
	}

	if (i == obj->hash_table)
		fprintf(fp, "%s" DBG_TAB "(empty)\n", prefix.c_str());

	fprintf(fp, "%s}\n", prefix.c_str());

	return;
}

void DBG_initPrintDebugInfo()
{
	traced_stack = vector<Ink_Object *>();
	return;
}

#define btos(b) ((b) ? "true" : "false")

inline string getTrapMask(Ink_FunctionAttribution attr)
{
	string ret = "";
	if (attr.hasTrap(INTER_RETURN)) {
		if (ret != "") ret += " | ";
		ret += "retn";
	}
	if (attr.hasTrap(INTER_CONTINUE)) {
		if (ret != "") ret += " | ";
		ret += "continue";
	}
	if (attr.hasTrap(INTER_BREAK)) {
		if (ret != "") ret += " | ";
		ret += "break";
	}
	return ret;
}

void printFunctionInfo(FILE *fp, Ink_FunctionObject *func, string prefix = "")
{
	unsigned int i;

	fprintf(fp, ", function attr: [\n");
	fprintf(fp, "%s" DBG_TAB "is native: %s\n", prefix.c_str(), btos(func->is_native));
	fprintf(fp, "%s" DBG_TAB "is inline: %s\n", prefix.c_str(), btos(func->is_inline));
	fprintf(fp, "%s" DBG_TAB "is generator: %s\n", prefix.c_str(), btos(func->is_generator));
	fprintf(fp, "%s" DBG_TAB "is partial applied: %s\n", prefix.c_str(), btos(func->partial_applied_argc > 0));

	fprintf(fp, "%s" DBG_TAB "parameter%s: (", prefix.c_str(), func->param.size() > 1 ? "s" : "");
	for (i = 0; i < func->param.size(); i++) {
		fprintf(fp, "%s%s%s%s",
				func->param[i].is_ref ? "&" : "",
				func->param[i].is_optional ? "*" : "",
				func->param[i].name ? func->param[i].name->c_str() : "",
				func->param[i].is_variant ? "..." : "");
		if (i < func->param.size() - 1) {
			fprintf(fp, ", ");
		}
	}
	fprintf(fp, ")\n");
	fprintf(fp, "%s" DBG_TAB "interrupt signal trap mask: %s\n", prefix.c_str(),
			getTrapMask(func->attr).c_str());
	fprintf(fp, "%s]", prefix.c_str());

	return;
}

void DBG_printDebugInfo(FILE *fp, Ink_Object *obj, string prefix, string slot_prefix, bool if_scan_slot)
{
	const char *slot_name = NULL;

	if (find(traced_stack.begin(), traced_stack.end(), obj) != traced_stack.end()) {
		fprintf(fp, "traced\n");
		return;
	}
	traced_stack.push_back(obj);

	if (obj) {
		slot_name = obj->getDebugName();
	}

	fprintf(fp, "%sobject@%lx of type \'%s\' in slot \'%s\'", prefix.c_str(), obj,
			obj ? getTypeName(obj->type) : "unpointed",
			(!slot_name || !strlen(slot_name) ? "anonymous slot" : slot_name));
	if (obj && obj->type == INK_FUNCTION) {
		printFunctionInfo(fp, as<Ink_FunctionObject>(obj), slot_prefix);
	}
	if (if_scan_slot)
		printSlotInfo(fp, obj, slot_prefix);
	else fprintf(fp, "\n");

	return;
}

void DBG_printTrace(FILE *fp, Ink_ContextChain *context, string prefix)
{
	Ink_ContextChain *i;
	Ink_ContextChain *inner_most = context->getLocal();

	fprintf(fp, "%srising from:\n", prefix.c_str());
	for (i = inner_most; i; i = i->outer) {
		fprintf(fp, DBG_TAB "line %d: ", i->getLineno());
		DBG_initPrintDebugInfo();
		DBG_printDebugInfo(false, fp, i->getCreater(), "", DBG_TAB);
	}

	return;
}