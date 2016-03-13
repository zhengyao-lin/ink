#include "engine.h"

namespace ink {

Ink_Object *Ink_InterpreteEngine::findConstant(wstring name)
{
	Ink_ConstantTable::iterator const_iter;

	if ((const_iter = const_table.find(name))
		!= const_table.end() && const_iter->second) {
		return const_iter->second->toObject(this);
	}

	return NULL;
}

Ink_Constant *Ink_InterpreteEngine::setConstant(wstring name, Ink_Object *obj)
{
	Ink_Constant *ret = obj ? obj->toConstant(this) : NULL;

	if (!ret) {
		assert(obj);
		InkWarn_Failed_Get_Constant(this, obj->type);
		return ret;
	}

	return const_table[name] = ret;
}

void Ink_InterpreteEngine::disposeConstant()
{
	Ink_ConstantTable::iterator const_iter;

	for (const_iter = const_table.begin();
		 const_iter != const_table.end(); const_iter++) {
		delete const_iter->second;
	}

	return;
}

}
