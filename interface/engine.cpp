#include "engine.h"

static Ink_InterpreteEngine *current_interprete_engine = NULL;

Ink_InterpreteEngine *Ink_getCurrentEngine()
{
	return current_interprete_engine;
}

void Ink_setCurrentEngine(Ink_InterpreteEngine *engine)
{
	current_interprete_engine = engine;
	return;
}

void Ink_InterpreteEngine::startParse(Ink_InputSetting setting)
{
	Ink_InterpreteEngine *backup = Ink_getCurrentEngine();
	Ink_setCurrentEngine(this);
	
	input_mode = INK_FILE_INPUT;
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

extern bool CGC_if_return;
extern bool CGC_if_yield;

Ink_Object *Ink_InterpreteEngine::execute(Ink_ContextChain *context)
{
	Ink_InterpreteEngine *backup = Ink_getCurrentEngine();
	Ink_setCurrentEngine(this);

	Ink_Object *ret;
	unsigned int i;

	if (!context) context = global_context;
	for (i = 0; i < top_level.size(); i++) {
		ret = top_level[i]->eval(context);
		if (CGC_if_return || CGC_if_yield) {
			break;
		}
		getCurrentGC()->checkGC();
	}

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