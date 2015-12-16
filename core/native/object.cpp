#include <sstream>
#include "../object.h"
#include "../context.h"
#include "../expression.h"
#include "native.h"

extern Ink_ExpressionList native_exp_list;

Ink_Object *InkNative_Object_Bond(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
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

Ink_Object *InkNative_Object_Debond(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	if (base->address && base->address->bondee) {
		base->address->bondee->bonding = NULL;
		return base->address->bondee->value;
	}

	return new Ink_NullObject();
}

Ink_Object *InkNative_Object_Not(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	return isTrue(base) ? new Ink_Numeric(0) : new Ink_Numeric(1);
}

bool isEqual(Ink_Object *a, Ink_Object *b)
{
	if (!(a && b) || a->type != b->type) return false;
	if (a->type == INK_NUMERIC) {
		return as<Ink_Numeric>(a)->value == as<Ink_Numeric>(b)->value;
	}
	if (a->type == INK_STRING) {
		return as<Ink_String>(a)->value == as<Ink_String>(b)->value;
	}
	if (a->type == INK_UNDEFINED && b->type == INK_UNDEFINED) return true;
	if (a->type == INK_NULL && b->type == INK_NULL) return true;

	return false;
}

Ink_Object *InkNative_Object_Equal(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	return new Ink_Numeric(isEqual(base, argc ? argv[0] : NULL));
}
Ink_Object *InkNative_Object_NotEqual(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	return new Ink_Numeric(!isEqual(base, argc ? argv[0] : NULL));
}

Ink_Object *InkNative_Object_Index(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	if (!checkArgument(argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}
	
	if (!base->getSlotMapping(as<Ink_String>(argv[0])->value.c_str())) {
		Ink_IdentifierExpression *id_exp = new Ink_IdentifierExpression(new string(as<Ink_String>(argv[0])->value.c_str()));
		native_exp_list.push_back(id_exp);
		return Ink_HashExpression::getSlot(base, id_exp->id->c_str());
	}

	return Ink_HashExpression::getSlot(base, as<Ink_String>(argv[0])->value.c_str());
}

Ink_Object *InkNative_Object_New(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");
	Ink_Object *new_obj = new Ink_Object();

	if (base->type == INK_FUNCTION) {
		return base->call(context, argc, argv, new_obj);
	}

	return new_obj;
}

Ink_Object *InkNative_Object_Clone(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot("base");

	return base->clone();
}

extern int object_native_method_table_count;
extern InkNative_MethodTable object_native_method_table[];

void Ink_Object::Ink_ObjectMethodInit()
{
	InkNative_MethodTable *table = object_native_method_table;
	int i, count = object_native_method_table_count;

	for (i = 0; i < count; i++) {
		setSlot(table[i].name, table[i].func);
	}
}