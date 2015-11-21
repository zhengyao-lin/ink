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
	int argi, j;
	Ink_HashTable *i;
	Ink_ContextObject *local = new Ink_ContextObject(); // new local context
	Ink_Object *ret_val = NULL;

	if (closure_context) context = closure_context;

	context->addContext(local); // add to context chain

	local->setSlot("base", getSlot("base"));
	local->setSlot("this", this);

	if (is_native) ret_val = native(context, argc, argv);
	else {
		for (i = arguments, argi = 0; i && argi < argc; i = i->next, argi++) {
			local->setSlot(i->key, argv[argi]->clone()); // initiate local argument
		}

		if (i || argi < argc) {
			InkWarn_Unfit_Argument();
		}

		for (j = 0; j < exp_list.size(); j++) {
			ret_val = exp_list[j]->eval(context); // eval each expression
			if (CGC_if_return) {
				if (!is_inline)
					CGC_if_return = false;
				break;
			}
		}
	}

	context->removeLast(); // delete the local environment

	return ret_val ? ret_val : new Ink_NullObject(); // return the last expression
}

Ink_FunctionObject::~Ink_FunctionObject()
{
	if (closure_context) Ink_ContextChain::disposeContextChain(closure_context);
	cleanHashTable();
	cleanHashTable(arguments);
}