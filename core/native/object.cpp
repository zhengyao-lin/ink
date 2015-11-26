#include <sstream>
#include "../object.h"
#include "../context.h"
#include "../expression.h"

extern Ink_ExpressionList native_exp_list;

Ink_Object *InkNative_Object_Bond(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");

	if (argc && base->address && argv[0]->address) {
		if (base->address == argv[0]->address) InkWarn_Self_Bonding();
		base->address->bonding = argv[0]->address;
		return argv[0];
	}

	InkWarn_Bonding_Failed();

	return new Ink_NullObject();
}

Ink_Object *InkNative_Object_Debond(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");

	if (base->address && base->address->bondee) {
		base->address->bondee->bonding = NULL;
		return base->address->bondee->value;
	}

	return new Ink_NullObject();
}

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
	}/* else if (argc && argv[0]->type == INK_INTEGER) {
		ostringstream ss;
		ss << "[" << as<Ink_Integer>(argv[0])->value << "]";
		if (!base->getSlotMapping(ss.str().c_str())) {
			Ink_IdentifierExpression *id_exp = new Ink_IdentifierExpression(new string(ss.str().c_str()));
			native_exp_list.push_back(id_exp);
			return Ink_HashExpression::getSlot(base, id_exp->id->c_str());
		}

		return Ink_HashExpression::getSlot(base, ss.str().c_str());
	}*/

	return new Ink_NullObject();
}

Ink_Object *InkNative_Object_New(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	Ink_Object *obj = base;
	Ink_Object *ret;

	if (base->type == INK_FUNCTION) {
		obj = base->call(context, argc, argv, true);
		if (obj->type == INK_FUNCTION) {
			ret = new Ink_Object(true);
			Ink_Object::cloneHashTable(obj, ret);
		} else ret = obj->clone();
		return ret;
	}

	return obj->clone();
}

Ink_Object *InkNative_Object_Clone(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv)
{
	Ink_Object *base = context->searchSlot("base");
	Ink_Object *ret;

	if (base->type == INK_FUNCTION) {
		ret = new Ink_Object(true);
		Ink_Object::cloneHashTable(base, ret);
		return ret;
	}

	return base->clone();
}

void Ink_Object::Ink_ObjectMethodInit()
{
	setSlot("->", new Ink_FunctionObject(InkNative_Object_Bond));
	setSlot("!!", new Ink_FunctionObject(InkNative_Object_Debond));
	setSlot("[]", new Ink_FunctionObject(InkNative_Object_Index));
	setSlot("new", new Ink_FunctionObject(InkNative_Object_New));
	setSlot("clone", new Ink_FunctionObject(InkNative_Object_Clone));
}