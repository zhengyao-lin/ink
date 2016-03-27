#include "context.h"
#include "interface/engine.h"

namespace ink {

Ink_ContextChain_sub *Ink_ContextChain::addContext(Ink_ContextObject *c)
{
	Ink_ContextChain_sub *new_c = new Ink_ContextChain_sub(c);
	
	if (tail) {
		tail->inner = new_c;
		new_c->outer = tail;
		tail = new_c;
	} else {
		head = tail = new_c;
	}

	return new_c;
}

void Ink_ContextChain::removeLast(bool if_delete)
{
	Ink_ContextChain_sub *tmp;

	if (tail) {
		if (tail->outer)
			tail->outer->inner = NULL;
		tmp = tail;
		tail = tail->outer;
		if (if_delete)
			delete tmp;
	}

	return;
}

void Ink_ContextChain::removeContext(Ink_ContextObject *c, bool if_delete)
{
	Ink_ContextChain_sub *i;

	for (i = tail; i; i = i->outer) {
		if (i->getContext() == c) {
			if (i->outer)
				i->outer->inner = i->inner;
			if (i->inner)
				i->inner->outer = i->outer;
			if (i == tail)
				tail = i->outer;
			if (i == head)
				head = i->inner;
			if (if_delete)
				delete i;
			break;
		}
	}
	return;
}

Ink_ContextObject *Ink_ContextChain::getGlobal()
{
	return head ? head->getContext() : NULL;
}

Ink_ContextObject *Ink_ContextChain::getLocal()
{
	return tail ? tail->getContext() : NULL;
}

Ink_Object *Ink_ContextChain::searchSlot(Ink_InterpreteEngine *engine, const char *slot_id)
{
	Ink_HashTable *ret = searchSlotMapping(engine, slot_id);

	return ret ? ret->getValue() : UNDEFINED;
}

Ink_HashTable *Ink_ContextChain::searchSlotMapping(Ink_InterpreteEngine *engine, const char *slot_id, Ink_ContextObject **found_in)
{
	Ink_ContextChain_sub *i = tail;
	Ink_HashTable *ret = NULL;

	while (i && !(ret = i->getContext()->getSlotMapping(engine, slot_id, false /* don't search prototype chain */))) {
		i = i->outer;
	}

	if (ret && i && found_in) {
		*found_in = i->getContext();
	}

	return ret;
}

void Ink_ContextChain::doSelfMark(Ink_InterpreteEngine *engine, IGC_Marker marker)
{
	Ink_ContextChain_sub *i;

	for (i = head; i; i = i->inner) {
		marker(engine, i->getContext());
	}

	return;
}

void Ink_ContextChain::disposeContextChain(Ink_ContextChain *chain)
{
	delete chain;
	return;
}

Ink_ContextChain *Ink_ContextChain::copyContextChain()
{
	Ink_ContextChain_sub *i;
	Ink_ContextChain *new_chain;

	if (head) {
		new_chain = new Ink_ContextChain(head->getContext());
		for (i = head->inner; i; i = i->inner) {
			new_chain->addContext(i->getContext());
		}
	} else {
		new_chain = new Ink_ContextChain();
	}

	return new_chain;
}

Ink_ContextChain *Ink_ContextChain::copyDeepContextChain(Ink_InterpreteEngine *engine)
{
	Ink_ContextChain_sub *i;
	Ink_ContextChain *new_chain;

	if (head) {
		if (!engine->cloneDeepHasTraced(head->getContext())) {
			new_chain = new Ink_ContextChain(as<Ink_ContextObject>(head->getContext()->cloneDeep(engine)));
		} else {
			new_chain = new Ink_ContextChain(NULL);
		}

		for (i = head->inner; i; i = i->inner) {
			if (!engine->cloneDeepHasTraced(i->getContext()))
				new_chain->addContext(as<Ink_ContextObject>(i->getContext()->cloneDeep(engine)));
			else
				new_chain->addContext(NULL);
		}
	} else {
		new_chain = new Ink_ContextChain();
	}

	return new_chain;
}

}
