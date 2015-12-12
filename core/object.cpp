#include <string.h>
#include "hash.h"
#include "object.h"
#include "expression.h"
#include "gc/collect.h"
#include "native/native.h"
#include "../interface/engine.h"
#include "coroutine/coroutine.h"

extern int numeric_native_method_table_count;
extern InkNative_MethodTable numeric_native_method_table[];
extern int string_native_method_table_count;
extern InkNative_MethodTable string_native_method_table[];
extern int object_native_method_table_count;
extern InkNative_MethodTable object_native_method_table[];
extern int array_native_method_table_count;
extern InkNative_MethodTable array_native_method_table[];
extern int function_native_method_table_count;
extern InkNative_MethodTable function_native_method_table[];

extern Ink_InterpreteEngine *current_interprete_engine;

Ink_Object *getMethod(Ink_Object *obj, const char *name, InkNative_MethodTable *table, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		if (!strcmp(name, table[i].name)) {
			return new Ink_FunctionObject(as<Ink_FunctionObject>(table[i].func)->native);
		}
	}
	return NULL;
}

Ink_Object *Ink_Object::getSlot(const char *key)
{
	Ink_HashTable *ret = getSlotMapping(key);

	return ret ? ret->value : new Ink_Undefined();
}

Ink_HashTable *Ink_Object::getSlotMapping(const char *key)
{
	Ink_HashTable *i;
	Ink_HashTable *ret = NULL;
	Ink_Object *method = NULL;

	for (i = hash_table; i; i = i->next) {
		if (!strcmp(i->key, key)) {
			for (ret = i; ret->bonding; ret = ret->bonding) ;
			ret->bondee = i;
			return ret;
		}
	}

	switch (type) {
		case INK_NUMERIC:
			method = getMethod(this, key, numeric_native_method_table,
							   numeric_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		case INK_STRING:
			method = getMethod(this, key, string_native_method_table,
							   string_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		case INK_ARRAY:
			method = getMethod(this, key, array_native_method_table,
							   array_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		case INK_FUNCTION:
			method = getMethod(this, key, function_native_method_table,
							   function_native_method_table_count);
			if (method) {
				ret = setSlot(key, method, false);
			}
			break;
		default: break;
	}
	if (!ret) {
		method = getMethod(this, key, object_native_method_table,
						   object_native_method_table_count);
		if (method)
		ret = setSlot(key, method, false);
	}

	return ret;
}

Ink_HashTable *Ink_Object::setSlot(const char *key, Ink_Object *value, bool if_check_exist)
{
	Ink_HashTable *slot = NULL;

	if (if_check_exist) slot = getSlotMapping(key);

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

Ink_Object *Ink_Numeric::clone()
{
	Ink_Object *new_obj = new Ink_Numeric(value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_String::clone()
{
	Ink_Object *new_obj = new Ink_String(value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *InkNative_Generator_Send(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	return NULL;
}

Ink_Object *Ink_FunctionObject::call(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv,
									 Ink_Object *this_p)
{
	unsigned int argi, j;
	// Ink_HashTable *i;
	Ink_ContextObject *local;
	Ink_Object *ret_val = NULL;
	IGC_CollectEngine *engine_backup = current_interprete_engine->getCurrentGC();

	if (is_generator) {
		Ink_Object *gen = new Ink_Object();
		Ink_FunctionObject *clone_origin = as<Ink_FunctionObject>(this->clone());
		clone_origin->is_generator = false;

		gen->setSlot("origin", clone_origin);

		gen->setSlot("address", new Ink_Array());
		gen->setSlot("send", new Ink_FunctionObject(InkNative_Generator_Send));

		return gen;
	}

	IGC_CollectEngine *gc_engine = new IGC_CollectEngine();
	IGC_initGC(gc_engine);

	local = new Ink_ContextObject();
	if (closure_context) context = closure_context->copyContextChain();
	if (!is_inline) { // if not inline function, set local context
		local->setSlot("base", getSlot("base"));
		local->setSlot("this", this_p ? this_p : this);
	}
	context->addContext(local);

	gc_engine->initContext(context);

	if (is_native) ret_val = native(context, argc, argv, this_p);
	else {
		for (j = 0, argi = 0; j < param.size(); j++, argi++) {
			local->setSlot(param[j]->c_str(), argi < argc ? argv[argi] : new Ink_Undefined()); // initiate local argument
			if (argi < argc)
				gc_engine->addPardon(argv[argi]);
		}

		if (argi > argc) {
			InkWarn_Unfit_Argument();
		}

		for (j = 0; j < exp_list.size(); j++) {
			gc_engine->checkGC();
			ret_val = exp_list[j]->eval(context); // eval each expression
			if (CGC_if_return) {
				if (!is_inline)
					CGC_if_return = false;
				break;
			}
		}
	}
	if (this_p) {
		ret_val = local->getSlot("this");
	}

	context->removeLast();
	
	if (ret_val)
		gc_engine->doMark(ret_val);
	gc_engine->collectGarbage();

	if (closure_context) Ink_ContextChain::disposeContextChain(context);

	if (engine_backup) engine_backup->link(gc_engine);

	IGC_initGC(engine_backup);
	delete gc_engine;

	return ret_val ? ret_val : new Ink_NullObject(); // return the last expression
}

Ink_FunctionObject::~Ink_FunctionObject()
{
	if (closure_context) Ink_ContextChain::disposeContextChain(closure_context);
	cleanHashTable();
}

Ink_ArrayValue Ink_Array::cloneArrayValue(Ink_ArrayValue val)
{
	Ink_ArrayValue ret = Ink_ArrayValue();
	unsigned int i;

	for (i = 0; i < val.size(); i++) {
		if (val[i])
			ret.push_back(new Ink_HashTable("", val[i]->value));
		else ret.push_back(NULL);
	}

	return ret;
}

Ink_Object *Ink_Array::clone()
{
	Ink_Object *new_obj = new Ink_Array(cloneArrayValue(value));

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_FunctionObject::clone()
{
	Ink_FunctionObject *new_obj = new Ink_FunctionObject();

	new_obj->is_native = is_native;
	new_obj->is_inline = is_inline;
	new_obj->is_generator = is_generator;
	new_obj->native = native;

	new_obj->param = param;
	new_obj->exp_list = exp_list;
	if (new_obj->closure_context)
		new_obj->closure_context = closure_context->copyContextChain();

	cloneHashTable(this, new_obj);

	return new_obj;
}