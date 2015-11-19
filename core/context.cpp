#include "context.h"

void Ink_ContextChain::addContext(Ink_ContextObject *c)
{
	Ink_ContextChain *new_context = new Ink_ContextChain(c);
	Ink_ContextChain *local = getLocal();

	local->inner = new_context;
	new_context->outer = local;
	return;
}

void Ink_ContextChain::removeLast()
{
	Ink_ContextChain *last = getLocal();

	if (last->outer)
		last->outer->inner = NULL;

	delete last;

	return;
}

Ink_ContextChain *Ink_ContextChain::getGlobal()
{
	Ink_ContextChain *i;
	for (i = this; i->outer; i = i->outer) ;
	return i;
}

Ink_ContextChain *Ink_ContextChain::getLocal()
{
	Ink_ContextChain *i;
	for (i = this; i->inner; i = i->inner) ;
	return i;
}

Ink_Object *Ink_ContextChain::searchSlot(const char *slot_id)
{
	Ink_ContextChain *local = getLocal();
	Ink_ContextChain *i = local;
	Ink_Object *ret = NULL;

	while (i && !(ret = i->context->getSlot(slot_id))) {
		i = i->outer;
	}

	return ret;
}

Ink_HashTable *Ink_ContextChain::searchSlotMapping(const char *slot_id)
{
	Ink_ContextChain *local = getLocal();
	Ink_ContextChain *i = local;
	Ink_HashTable *ret = NULL;

	while (i && !(ret = i->context->getSlotMapping(slot_id))) {
		i = i->outer;
	}

	return ret;
}