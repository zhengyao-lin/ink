#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <stdio.h>
#include <string.h>
#include "../core/hash.h"
#include "../core/object.h"
#include "../core/expression.h"
#include "../core/context.h"
#include "../core/gc/collect.h"

class Ink_InterpreteEngine;

extern Ink_InterpreteEngine *current_interprete_engine;
extern FILE *yyin;
int yyparse();
int yylex_destroy();
void Ink_GlobalMethodInit(Ink_ContextChain *context);
void Ink_setStringInput(const char **source);
void Ink_setCurrentEngine(Ink_InterpreteEngine *engine);
Ink_InterpreteEngine *Ink_getCurrentEngine();

typedef enum {
	INK_FILE_INPUT,
	INK_STRING_INPUT
} Ink_InputMode;

class Ink_InterpreteEngine {
public:
	Ink_ExpressionList top_level;
	IGC_CollectEngine *gc_engine;
	Ink_ContextChain *global_context;
	Ink_InputMode input_mode;

	Ink_ContextChain *trace;

	long igc_object_count;
	long igc_collect_treshold;
	IGC_CollectEngine *current_gc_engine;
	int igc_mark_period;

	Ink_InterpreteEngine()
	{
		Ink_InterpreteEngine *backup = Ink_getCurrentEngine();
		Ink_setCurrentEngine(this);

		igc_object_count = 0;
		igc_collect_treshold = IGC_COLLECT_TRESHOLD;
		igc_mark_period = 1;

		gc_engine = new IGC_CollectEngine();
		current_gc_engine = gc_engine;
		global_context = new Ink_ContextChain(new Ink_ContextObject());
		trace = new Ink_ContextChain(global_context->context);
		gc_engine->initContext(global_context);

		global_context->context->setSlot("this", global_context->context);
		Ink_GlobalMethodInit(global_context);

		Ink_setCurrentEngine(backup);
	}

	void startParse(FILE *input = stdin);
	void startParse(string code);
	Ink_Object *execute(Ink_ContextChain *context = NULL);
	static void cleanExpressionList(Ink_ExpressionList exp_list);
	static void cleanContext(Ink_ContextChain *context);

	~Ink_InterpreteEngine()
	{
		Ink_InterpreteEngine *backup = Ink_getCurrentEngine();
		Ink_setCurrentEngine(this);

		gc_engine->collectGarbage(true);
		delete gc_engine;

		cleanExpressionList(top_level);
		cleanContext(global_context);
		cleanContext(trace);

		Ink_setCurrentEngine(backup);
	}
};

#endif