#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include <string.h>
#include "setting.h"

using namespace std;

class Ink_Object;
class Ink_Expression;
class Ink_InterpreteEngine;
class IGC_CollectEngine;
class Ink_ContextObject;
class Ink_ContextChain;

typedef vector<Ink_Expression *> Ink_ExpressionList;

extern FILE *yyin;
int yyparse();
int yylex_destroy();
void Ink_GlobalMethodInit(Ink_ContextChain *context);
void Ink_setStringInput(const char **source);
Ink_InterpreteEngine *Ink_getCurrentEngine();
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

	// MutexLock gc_lock;
	long igc_object_count;
	long igc_collect_treshold;
	IGC_CollectEngine *current_gc_engine;
	int igc_mark_period;
	// std::map<int, IGC_CollectEngine *> gc_engine_map;

	Ink_InterpreteEngine();

	Ink_ContextChain *addTrace(Ink_ContextObject *context);
	void removeLastTrace();

	inline int setCurrentGC(IGC_CollectEngine *engine)
	{
		current_gc_engine = engine;
		return 0;
	}

	inline IGC_CollectEngine *getCurrentGC()
	{
		return current_gc_engine;
	}

	void startParse(Ink_InputSetting setting);
	void startParse(FILE *input = stdin, bool close_fp = false);
	void startParse(string code);
	Ink_Object *execute(Ink_ContextChain *context = NULL);
	Ink_Object *execute(Ink_Expression *exp);
	static void cleanExpressionList(Ink_ExpressionList exp_list);
	static void cleanContext(Ink_ContextChain *context);

	~Ink_InterpreteEngine();
};

#endif