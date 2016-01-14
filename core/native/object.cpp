#include <sstream>
#include "../object.h"
#include "../context.h"
#include "../expression.h"
#include "../general.h"
#include "native.h"
#include "../interface/engine.h"

Ink_Object *InkNative_Object_Bond(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	if (argc && base->address && argv[0]->address) {
		if (base->address == argv[0]->address) InkWarn_Self_Bonding();
		base->address->bonding = argv[0]->address;
		return argv[0];
	}

	InkWarn_Bonding_Failed();

	return NULL_OBJ;
}

Ink_Object *InkNative_Object_Debond(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	if (base->address && base->address->bondee) {
		base->address->bondee->bonding = NULL;
		return base->address->bondee->getValue();
	}

	return NULL_OBJ;
}

Ink_Object *InkNative_Object_Not(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	return isTrue(base) ? new Ink_Numeric(engine, 0) : new Ink_Numeric(engine, 1);
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

Ink_Object *InkNative_Object_Equal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	return new Ink_Numeric(engine, isEqual(base, argc ? argv[0] : NULL));
}

Ink_Object *InkNative_Object_NotEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	return new Ink_Numeric(engine, !isEqual(base, argc ? argv[0] : NULL));
}

Ink_Object *InkNative_Object_Index(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	if (!checkArgument(argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}
	
	if (!base->getSlotMapping(engine, as<Ink_String>(argv[0])->value.c_str())) {
		Ink_IdentifierExpression *id_exp = new Ink_IdentifierExpression(new string(as<Ink_String>(argv[0])->value.c_str()));
		engine->native_exp_list.push_back(id_exp);
		return getSlotWithProto(engine, context, base, id_exp->id->c_str());
	}

	return getSlotWithProto(engine, context, base, as<Ink_String>(argv[0])->value.c_str());
}

Ink_Object *InkNative_Object_New(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_Object *new_obj = new Ink_Object(engine);

	if (base->type == INK_FUNCTION) {
		return base->call(engine, context, argc, argv, new_obj);
	}

	return new_obj;
}

Ink_Object *InkNative_Object_Delete(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	if (!base->address) {
		InkWarn_Delete_Function_Argument_Require();
		return NULL_OBJ;
	}
	base->address->setValue(NULL);

	return NULL_OBJ;
}

Ink_Object *InkNative_Object_Clone(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	return base->clone(engine);
}

const char *createStrConstant(Ink_InterpreteEngine *engine, const char *str)
{
	string *tmp;
	engine->native_exp_list.push_back(new Ink_StringConstant(tmp = new string(str)));
	return tmp->c_str();
}

Ink_Object *InkNative_Object_SetGetter(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_HashTable *hash;
	const char *tmp;

	if (!checkArgument(argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	tmp = as<Ink_String>(argv[0])->value.c_str();
	if (!(hash = base->getSlotMapping(engine, tmp))) {
		hash = base->setSlot(createStrConstant(engine, tmp), NULL);
	}

	hash->getter = argc > 1 ? argv[1] : NULL;

	return NULL_OBJ;
}

Ink_Object *InkNative_Object_SetSetter(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_HashTable *hash;
	const char *tmp;

	if (!checkArgument(argc, argv, 1, INK_STRING)) {
		return NULL_OBJ;
	}

	tmp = as<Ink_String>(argv[0])->value.c_str();
	if (!(hash = base->getSlotMapping(engine, tmp))) {
		hash = base->setSlot(createStrConstant(engine, tmp), NULL);
	}

	hash->setter = argc > 1 ? argv[1] : NULL;

	return NULL_OBJ;
}

void cleanArrayHashTable(Ink_ArrayValue val);

Ink_Object *InkNative_Object_Each(Ink_InterpreteEngine *engine, Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_Object **args;
	Ink_Object *ret_tmp;
	Ink_HashTable *hash;
	Ink_ArrayValue ret_val;

	if (!checkArgument(argc, argv, 1, INK_FUNCTION)) {
		return NULL_OBJ;
	}

	args = (Ink_Object **)malloc(2 * sizeof(Ink_Object *));
	for (hash = base->hash_table; hash; hash = hash->next) {
		args[0] = new Ink_String(engine, hash->key);
		args[1] = hash->getValue() ? hash->getValue() : UNDEFINED;
		ret_val.push_back(new Ink_HashTable(ret_tmp = argv[0]->call(engine, context, 2, args)));
		switch (engine->CGC_interrupt_signal) {
			case INTER_RETURN:
				free(args);
				cleanArrayHashTable(ret_val);
				return engine->CGC_interrupt_value; // signal penetrated
			case INTER_DROP:
			case INTER_BREAK:
				return trapSignal(engine); // trap the signal
			case INTER_CONTINUE:
				trapSignal(engine); // trap the signal, but do not return
				continue;
			default: ;
		}
	}
	free(args);

	return new Ink_Array(engine, ret_val);
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
