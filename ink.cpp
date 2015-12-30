#include <typeinfo>
#include "core/hash.h"
#include "core/object.h"
#include "core/expression.h"
#include "core/debug.h"
#include "core/package/load.h"
#include "core/gc/collect.h"
#include "core/thread/thread.h"
#include "core/coroutine/coroutine.h"
#include "interface/engine.h"
#include "interface/setting.h"

Ink_ExpressionList native_exp_list = Ink_ExpressionList();

void cleanAll()
{
	unsigned int i;
	for (i = 0; i < native_exp_list.size(); i++) {
		delete native_exp_list[i];
	}
}

/*void *test1(void *p)
{
	registerThread();
	printf("1: %u\n", getThreadID());
}

void *test2(void *p)
{
	registerThread();
	printf("2: %u\n", getThreadID());
}

void *test3(void *p)
{
	registerThread();
	printf("3: %u\n", getThreadID());
}*/

int main(int argc, char **argv)
{
#if 0
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
		gc_engine->checkGC();
	}

	// func=(){global=120;};
	// context->searchSlot("func")->call(context, 0, NULL);
	// Ink_Object *out = context->searchSlot("global");
	// if (defined(out) && out->type == INK_NUMERIC)
	// 	printf("global: %d\n", as<Ink_Numeric>(out)->value);

	gc_engine->collectGarbage(true);
	delete gc_engine;
#endif
	// initThread();
	// registerThread();

	DBG_initTypeMapping();

	Ink_InterpreteEngine *engine = new Ink_InterpreteEngine();
	engine->startParse(Ink_InputSetting::parseArg(argc, argv));

	engine->execute();

	// joinAllThread();

	delete engine;
	cleanAll();
	DBG_disposeTypeMapping();
	closeAllHandler();

	return 0;
}