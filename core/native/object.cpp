#include <sstream>
#include "../object.h"
#include "../context.h"
#include "../expression.h"

extern Ink_ExpressionList native_exp_list;

Ink_Object *InkNative_Object_Index(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");

	if (argc && argv[0]->type == INK_STRING) {
		if (!base->getSlotMapping(as<Ink_String>(argv[0])->value.c_str())) {
			Ink_IdentifierExpression *id_exp = new Ink_IdentifierExpression(new string(as<Ink_String>(argv[0])->value.c_str()));
			native_exp_list.push_back(id_exp);
			return Ink_HashExpression::getSlot(base, id_exp->id->c_str());
		}

		return Ink_HashExpression::getSlot(base, as<Ink_String>(argv[0])->value.c_str());
	} else if (argc && argv[0]->type == INK_INTEGER) {
		ostringstream ss;
		ss << "[" << as<Ink_Integer>(argv[0])->value << "]";
		if (!base->getSlotMapping(ss.str().c_str())) {
			Ink_IdentifierExpression *id_exp = new Ink_IdentifierExpression(new string(ss.str().c_str()));
			native_exp_list.push_back(id_exp);
			return Ink_HashExpression::getSlot(base, id_exp->id->c_str());
		}

		return Ink_HashExpression::getSlot(base, ss.str().c_str());
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Object_Clone(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	return base->clone();
}

void Ink_Object::Ink_ObjectMethodInit()
{
	setSlot("[]", new Ink_FunctionObject(InkNative_Object_Index));
	setSlot("clone", new Ink_FunctionObject(InkNative_Object_Clone));
}