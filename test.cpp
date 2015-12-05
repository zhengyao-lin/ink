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

Ink_Object *print(Ink_ContextChain *context, unsigned int argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (argv[0]->type == INK_NUMERIC)
		printf("print(numeric): %lf\n", as<Ink_Numeric>(argv[0])->value);
	else if (argv[0]->type == INK_STRING)
		printf("%s\n", as<Ink_String>(argv[0])->value.c_str());
	else if (argv[0]->type == INK_NULL)
		printf("(null)\n");
	else if (argv[0]->type == INK_UNDEFINED)
		printf("(undefined)\n");
	else
		printf("print: non-printable type: %d\n", argv[0]->type);

	return new Ink_NullObject();
}

void cleanAll(Ink_ContextChain *context)
{
	unsigned int i;
	for (i = 0; i < exp_list.size(); i++) {
		delete exp_list[i];
	}
	for (i = 0; i < native_exp_list.size(); i++) {
		delete native_exp_list[i];
	}
	// IGC_collectGarbage(context, true);
	cleanContext(context);
}

void Ink_GlobalMethodInit(Ink_ContextChain *context);

int main()
{
	yyparse();
	/*Ink_Object *obj = new Ink_Object();
	Ink_Numeric *slot_val = new Ink_Numeric(102);

	obj->setSlot("hi", slot_val);
	printf("%d\n", obj->getSlot("hi")->type);
	printf("%d\n", obj->type);

	delete obj;
	delete slot_val;*/
	/* printf("IGC_CollectEngine: %d\nInk_ContextObject: %d\nInk_ContextChain: %d\nInk_FunctionObject: %d\n",
			sizeof(IGC_CollectEngine),
			sizeof(Ink_ContextObject),
			sizeof(Ink_ContextChain),
			sizeof(Ink_FunctionObject)); */

	unsigned int i;
	IGC_CollectEngine *gc_engine = new IGC_CollectEngine();
	IGC_initGC(gc_engine, true);

	Ink_ContextChain *context = new Ink_ContextChain(new Ink_ContextObject());
	gc_engine->initContext(context);

	context->context->setSlot("p", new Ink_FunctionObject(print));
	context->context->setSlot("this", context->context);
	Ink_GlobalMethodInit(context);

	for (i = 0; i < exp_list.size(); i++) {
		exp_list[i]->eval(context);
		// IGC_collectGarbage(context);
		gc_engine->checkGC();
	}

	// func=(){global=120;};
	// context->searchSlot("func")->call(context, 0, NULL);
	// Ink_Object *out = context->searchSlot("global");
	// if (defined(out) && out->type == INK_NUMERIC)
	// 	printf("global: %d\n", as<Ink_Numeric>(out)->value);

	gc_engine->collectGarbage(true);
	delete gc_engine;
	cleanAll(context);
	yylex_destroy();

	return 0;
}