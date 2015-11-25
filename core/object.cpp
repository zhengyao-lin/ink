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
			Ink_HashTable *ret;
			for (ret = i; ret->bonding; ret = ret->bonding) ;
			return ret;
		}
	}
	return NULL;
}

Ink_HashTable *Ink_Object::setSlot(const char *key, Ink_Object *value)
{
	Ink_HashTable *slot = getSlotMapping(key);
	if (slot) {
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

void Ink_Object::deleteSlot(const char *key)
{
	Ink_HashTable *i, *prev;

	for (i = hash_table, prev = NULL; i; prev = i, i = i->next) {
		if (!strcmp(i->key, key)) {
			if (prev) {
				prev->next = i->next;
				delete i;
				return;
			} else {
				hash_table = i->next;
				delete i;
			}
		}
	}

	return;
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
		dest->setSlot(i->key, i->value);
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

Ink_Object *Ink_String::clone()
{
	Ink_Object *new_obj = new Ink_String(value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_FunctionObject::call(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, bool return_this)
{
	unsigned int argi, j;
	Ink_HashTable *i;
	Ink_ContextObject *local; // new local context
	Ink_Object *ret_val = NULL;

	local = new Ink_ContextObject();
	if (!is_inline) { // if not inline function, set local context
		if (closure_context) context = closure_context;

		local->setSlot("base", getSlot("base"));
		local->setSlot("this", this);
	}
	context->addContext(local);

	if (is_native) ret_val = native(context, argc, argv);
	else {
		for (i = arguments, argi = 0; i && argi < argc; i = i->next, argi++) {
			local->setSlot(i->key, argv[argi]); // initiate local argument
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
		if (return_this) {
			ret_val = local->getSlot("this");
		}
	}

	if (!is_inline)
		context->removeLast(); // delete the local environment

	return ret_val ? ret_val : new Ink_NullObject(); // return the last expression
}

Ink_FunctionObject::~Ink_FunctionObject()
{
	if (closure_context) Ink_ContextChain::disposeContextChain(closure_context);
	cleanHashTable();
	cleanHashTable(arguments);
}

Ink_ArrayValue Ink_Array::cloneArrayValue(Ink_ArrayValue val)
{
	Ink_ArrayValue ret = Ink_ArrayValue();
	unsigned int i;

	for (i = 0; i < val.size(); i++) {
		if (val[i])
			ret.push_back(new Ink_HashTable("", val[i]->value));
	}

	return ret;
}

Ink_Object *Ink_Array::clone()
{
	Ink_Object *new_obj = new Ink_Array(cloneArrayValue(value));

	cloneHashTable(this, new_obj);

	return new_obj;
}