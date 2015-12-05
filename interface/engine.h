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

	Ink_InterpreteEngine()
	{
		gc_engine = new IGC_CollectEngine();
		IGC_initGC(gc_engine, true);
		global_context = new Ink_ContextChain(new Ink_ContextObject());
		gc_engine->initContext(global_context);

		global_context->context->setSlot("this", global_context->context);
		Ink_GlobalMethodInit(global_context);
	}

	void startParse(FILE *input = stdin);
	void startParse(string code);
	Ink_Object *execute();
	void cleanTopLevel();
	static void cleanContext(Ink_ContextChain *context);

	~Ink_InterpreteEngine()
	{
		gc_engine->collectGarbage(true);
		delete gc_engine;
		cleanTopLevel();
		cleanContext(global_context);
	}
};

#endif