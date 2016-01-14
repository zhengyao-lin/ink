#include "context.h"

Ink_ContextChain *Ink_ContextChain::addContext(Ink_ContextObject *c)
{
	Ink_ContextChain *new_context = new Ink_ContextChain(c);
	Ink_ContextChain *local = getLocal();

	local->inner = new_context;
	new_context->outer = local;

	return new_context;
}

void Ink_ContextChain::removeLast(bool if_delete)
{
	Ink_ContextChain *last = getLocal();

	if (last->outer)
		last->outer->inner = NULL;

	if (if_delete)
		delete last;

	return;
}

Ink_ContextChain *Ink_ContextChain::getGlobal()
{
	Ink_ContextChain *i;
	for (i = this; i && i->outer; i = i->outer) ;
	return i;
}

Ink_ContextChain *Ink_ContextChain::getLocal()
{
	Ink_ContextChain *i;
	for (i = this; i && i->inner; i = i->inner) ;
	return i;
}

Ink_Object *Ink_ContextChain::searchSlot(Ink_InterpreteEngine *engine, const char *slot_id)
{
	Ink_ContextChain *local = getLocal();
	Ink_ContextChain *i = local;
	Ink_Object *ret = NULL;

	while (i && !(ret = i->context->getSlot(engine, slot_id))) {
		i = i->outer;
	}

	return ret;
}

Ink_HashTable *Ink_ContextChain::searchSlotMapping(Ink_InterpreteEngine *engine, const char *slot_id)
{
	Ink_ContextChain *local = getLocal();
	Ink_ContextChain *i = local;
	Ink_HashTable *ret = NULL;

	while (i && !(ret = i->context->getSlotMapping(engine, slot_id))) {
		i = i->outer;
	}

	return ret;
}

void Ink_ContextChain::disposeContextChain(Ink_ContextChain *head)
{
	Ink_ContextChain *i, *tmp;

	if (head && head->outer) {
		head->outer = NULL;
	}

	for (i = head; i;) {
		tmp = i;
		i = i->inner;
		delete tmp;
	}

	return;
}

Ink_ContextChain *Ink_ContextChain::copyContextChain()
{
	Ink_ContextChain *head = getGlobal();
	Ink_ContextChain *i, *new_chain;

	new_chain = new Ink_ContextChain(head->context);
	for (i = head->inner; i; i = i->inner) {
		new_chain->addContext(i->context);
	}

	return new_chain;
}
