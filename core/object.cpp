#include "hash.h"
#include "object.h"
#include "expression.h"

Ink_Object *Ink_Object::getSlot(const char *key)
{
	Ink_HashTable *ret;
	return (ret = getSlotMapping(key)) ? ret->value : new Ink_Undefined();
}

Ink_HashTable *Ink_Object::getSlotMapping(const char *key)
{
	Ink_HashTable *i;
	for (i = hash_table; i; i = i->next) {
		if (!strcmp(i->key, key)){
			return i;
		}
	}
	return NULL;
}

Ink_HashTable *Ink_Object::setSlot(const char *key, Ink_Object *value)
{
	Ink_HashTable *slot;
	if (slot = getSlotMapping(key)) {
		slot->value = value;
	} else {
		slot = new Ink_HashTable(key, value);
		if (hash_table)
			hash_table->getEnd()->next = slot;
		else
			hash_table = slot;
	}

	return slot;
}

void Ink_Object::cleanHashTable()
{
	cleanHashTable(hash_table);
	hash_table = NULL;

	return;
}

void Ink_Object::cleanHashTable(Ink_HashTable *table)
{
	Ink_HashTable *i, *tmp;
	for (i = table; i;) {
		tmp = i;
		i = i->next;
		delete tmp;
	}

	return;
}

void Ink_Object::cloneHashTable(Ink_Object *src, Ink_Object *dest)
{
	Ink_HashTable *i;
	for (i = src->hash_table; i; i = i->next) {
		dest->setSlot(i->key, i->value->type != INK_FUNCTION ? i->value->clone() : i->value);
	}

	return;
}

Ink_Object *Ink_Object::clone()
{
	Ink_Object *new_obj = new Ink_Object();

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_Integer::clone()
{
	Ink_Object *new_obj = new Ink_Integer(value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_FunctionObject::call(Ink_ContextChain *context, int argc, Ink_Object **argv)
{
	if (is_native) return native(context, argc, argv);
	int argi, j;
	Ink_HashTable *i;
	Ink_ContextObject *local = new Ink_ContextObject(); // new local context
	Ink_Object *ret_val = NULL;

	context->addContext(local); // add to context chain
	for (i = arguments, argi = 0; i && argi < argc; i = i->next, argi++) {
		local->setSlot(i->key, argv[argi]->type != INK_FUNCTION ? argv[argi]->clone() : argv[argi]); // initiate local argument
	}

	for (j = 0; j < exp_list.size(); j++) {
		ret_val = exp_list[j]->eval(context); // eval each expression
	}

	context->removeLast(); // delete the local environment

	return ret_val ? ret_val : new Ink_NullObject(); // return the last expression
}