#include <typeinfo>
#include "core/hash.h"
#include "core/object.h"
#include "core/expression.h"
#include "core/gc/collect.h"

Ink_ExpressionList exp_list = Ink_ExpressionList();
Ink_ExpressionList native_exp_list = Ink_ExpressionList();

extern int yyparse();
extern int yylex_destroy();

void cleanContext(Ink_ContextChain *context)
{
	Ink_ContextChain *i, *tmp;
	for (i = context->getGlobal(); i;) {
		tmp = i;
		i = i->inner;
		delete tmp;
	}

	return;
}

bool defined(Ink_Object *obj)
{
	return obj->type != INK_UNDEFINED;
}

Ink_Object *print(Ink_ContextChain *context, int argc, Ink_Object **argv)
{
	if (argv[0]->type == INK_INTEGER)
		printf("print(integer): %d\n", as<Ink_Integer>(argv[0])->value);

	return new Ink_NullObject();
}

void cleanAll(Ink_ContextChain *context)
{
	int i;
	for (i = 0; i < exp_list.size(); i++) {
		delete exp_list[i];
	}
	for (i = 0; i < native_exp_list.size(); i++) {
		delete native_exp_list[i];
	}
	IGC_collectGarbage(context, true);
	cleanContext(context);
}

void Ink_GlobalMethodInit(Ink_ContextChain *context);

int main()
{
	yyparse();
	/*Ink_Object *obj = new Ink_Object();
	Ink_Integer *slot_val = new Ink_Integer(102);

	obj->setSlot("hi", slot_val);
	printf("%d\n", obj->getSlot("hi")->type);
	printf("%d\n", obj->type);

	delete obj;
	delete slot_val;*/

	int i;
	Ink_ContextChain *context = new Ink_ContextChain(new Ink_ContextObject());

	context->context->setSlot("p", new Ink_FunctionObject(print));
	Ink_GlobalMethodInit(context);

	IGC_initGC(context);

	for (i = 0; i < exp_list.size(); i++) {
		Ink_Object *ret_val = exp_list[i]->eval(context);
		// IGC_collectGarbage(context);
	}

	// func=(){global=120;};
	// context->searchSlot("func")->call(context, 0, NULL);
	// Ink_Object *out = context->searchSlot("global");
	// if (defined(out) && out->type == INK_INTEGER)
	// 	printf("global: %d\n", as<Ink_Integer>(out)->value);

	cleanAll(context);

	return 0;
}