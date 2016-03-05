#include "engine.h"

namespace ink {

void Ink_InterpreteEngine::initTypeMapping()
{
	Ink_TypeTag i;

	dbg_type_mapping = vector<DBG_TypeMapping *>();

	for (i = 0; i < INK_LAST; i++) {
		dbg_type_mapping.push_back(new DBG_TypeMapping(i, dbg_fixed_type_mapping[i].name));
	}

	return;
}

void Ink_InterpreteEngine::disposeTypeMapping()
{
	vector<DBG_TypeMapping *>::iterator type_iter;

	for (type_iter = dbg_type_mapping.begin();
		 type_iter != dbg_type_mapping.end(); type_iter++) {
		delete *type_iter;
	}
	dbg_type_mapping = vector<DBG_TypeMapping *>();

	return;
}

Ink_TypeTag Ink_InterpreteEngine::registerType(const char *name)
{
	string *tmp_str = new string(name);
	Ink_TypeTag ret = dbg_type_mapping.size();
	dbg_type_mapping.push_back(new DBG_TypeMapping(ret, tmp_str->c_str(), tmp_str));
	return ret;
}

const char *Ink_InterpreteEngine::getTypeName(Ink_TypeTag type_tag)
{
	return dbg_type_mapping[type_tag]->name;
}

}
