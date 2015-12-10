#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <map>
#include <stdio.h>
#include <string.h>
#include "../core/hash.h"
#include "../core/object.h"
#include "../core/expression.h"
#include "../core/context.h"
#include "../core/gc/collect.h"
#include "../core/thread/thread.h"

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

	// MutexLock gc_lock;
	long igc_object_count;
	long igc_collect_treshold;
	IGC_CollectEngine *current_gc_engine;
	int igc_mark_period;
	// std::map<int, IGC_CollectEngine *> gc_engine_map;

	Ink_InterpreteEngine()
	{
		// gc_lock.init();
		Ink_setCurrentEngine(this);

		igc_object_count = 0;
		igc_collect_treshold = IGC_COLLECT_TRESHOLD;
		igc_mark_period = 1;

		gc_engine = new IGC_CollectEngine();
		setCurrentGC(gc_engine);
		global_context = new Ink_ContextChain(new Ink_ContextObject());
		gc_engine->initContext(global_context);

		global_context->context->setSlot("this", global_context->context);
		Ink_GlobalMethodInit(global_context);
	}

	int setCurrentGC(IGC_CollectEngine *engine)
	{
		#if 0
		int id;

		// gc_lock.lock();
		gc_engine_map[id = getThreadID()] = engine;
		// gc_lock.unlock();

		return id;
		#endif
		current_gc_engine = engine;
		return 0;
	}

	IGC_CollectEngine *getCurrentGC()
	{
		#if 0
		IGC_CollectEngine *ret;

		// gc_lock.lock();
		ret = gc_engine_map[getThreadID()];
		// gc_lock.unlock();

		return ret;
		#endif
		return current_gc_engine;
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

		Ink_setCurrentEngine(backup);
	}
};

#endif