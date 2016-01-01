#include "engine.h"
#include "core/gc/collect.h"
#include "core/hash.h"
#include "core/object.h"
#include "core/expression.h"
#include "core/general.h"
#include "core/thread/thread.h"

static Ink_InterpreteEngine *current_interprete_engine = NULL;
extern InterruptSignal CGC_interrupt_signal;
extern Ink_CodeMode CGC_code_mode;

Ink_InterpreteEngine *Ink_getCurrentEngine()
{
	return current_interprete_engine;
}

void Ink_setCurrentEngine(Ink_InterpreteEngine *engine)
{
	current_interprete_engine = engine;
	return;
}

Ink_InterpreteEngine::Ink_InterpreteEngine()
{
	// gc_lock.init();
	Ink_setCurrentEngine(this);

	igc_object_count = 0;
	igc_collect_treshold = IGC_COLLECT_TRESHOLD;
	igc_mark_period = 1;
	trace = NULL;

	gc_engine = new IGC_CollectEngine();
	setCurrentGC(gc_engine);
	global_context = new Ink_ContextChain(new Ink_ContextObject());
	// gc_engine->initContext(global_context);

	global_context->context->setSlot("this", global_context->context);
	Ink_GlobalMethodInit(global_context);

	addTrace(global_context->context);
}

Ink_ContextChain *Ink_InterpreteEngine::addTrace(Ink_ContextObject *context)
{
	if (!trace) return trace = new Ink_ContextChain(context);
	return trace->addContext(context);
}

void Ink_InterpreteEngine::removeLastTrace()
{
	if (trace)
		trace->removeLast();
	return;
}

void Ink_InterpreteEngine::startParse(Ink_InputSetting setting)
{
	Ink_InterpreteEngine *backup = Ink_getCurrentEngine();
	Ink_setCurrentEngine(this);
	
	input_mode = INK_FILE_INPUT;
	CGC_code_mode = setting.getMode();
	// cleanTopLevel();
	top_level = Ink_ExpressionList();
	yyin = setting.getInput();
	yyparse();
	yylex_destroy();

	setting.clean();

	Ink_setCurrentEngine(backup);

	return;
}

void Ink_InterpreteEngine::startParse(FILE *input, bool close_fp)
{
	Ink_InterpreteEngine *backup = Ink_getCurrentEngine();
	Ink_setCurrentEngine(this);
	
	input_mode = INK_FILE_INPUT;
	// cleanTopLevel();
	top_level = Ink_ExpressionList();
	yyin = input;
	yyparse();
	yylex_destroy();

	if (close_fp) fclose(input);

	Ink_setCurrentEngine(backup);

	return;
}

void Ink_InterpreteEngine::startParse(string code)
{
	Ink_InterpreteEngine *backup = Ink_getCurrentEngine();
	Ink_setCurrentEngine(this);

	const char **input = (const char **)malloc(2 * sizeof(char *));

	input[0] = code.c_str();
	input[1] = NULL;
	input_mode = INK_STRING_INPUT;
	// cleanTopLevel();
	top_level = Ink_ExpressionList();
	
	Ink_setStringInput(input);
	yyparse();
	yylex_destroy();

	free(input);

	Ink_setCurrentEngine(backup);

	return;
}

Ink_Object *Ink_InterpreteEngine::execute(Ink_ContextChain *context)
{
	Ink_InterpreteEngine *backup = Ink_getCurrentEngine();
	Ink_setCurrentEngine(this);

	Ink_Object *ret;
	unsigned int i;

	if (!context) context = global_context;
	for (i = 0; i < top_level.size(); i++) {
		getCurrentGC()->checkGC();
		ret = top_level[i]->eval(context);
		if (CGC_interrupt_signal == INTER_RETURN) {
			break;
		}
	}

	Ink_setCurrentEngine(backup);

	return ret;
}

Ink_Object *Ink_InterpreteEngine::execute(Ink_Expression *exp)
{
	Ink_InterpreteEngine *backup = Ink_getCurrentEngine();
	Ink_setCurrentEngine(this);

	Ink_Object *ret;
	Ink_ContextChain *context = NULL;

	if (!context) context = global_context;
	getCurrentGC()->checkGC();
	ret = exp->eval(context);

	Ink_setCurrentEngine(backup);

	return ret;
}

void Ink_InterpreteEngine::cleanExpressionList(Ink_ExpressionList exp_list)
{
	unsigned int i;

	for (i = 0; i < exp_list.size(); i++) {
		delete exp_list[i];
	}

	return;
}

void Ink_InterpreteEngine::cleanContext(Ink_ContextChain *context)
{
	Ink_ContextChain *i, *tmp;
	for (i = context->getGlobal(); i;) {
		tmp = i;
		i = i->inner;
		delete tmp;
	}

	return;
}

Ink_InterpreteEngine::~Ink_InterpreteEngine()
{
	Ink_InterpreteEngine *backup = Ink_getCurrentEngine();
	Ink_setCurrentEngine(this);

	gc_engine->collectGarbage(true);
	delete gc_engine;

	cleanExpressionList(top_level);
	cleanContext(global_context);
	cleanContext(trace);
	StrPool_dispose();

	Ink_setCurrentEngine(backup);
}