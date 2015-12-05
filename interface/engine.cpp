#include "engine.h"

void Ink_InterpreteEngine::startParse(FILE *input)
{
	input_mode = INK_FILE_INPUT;
	current_interprete_engine = this;
	cleanTopLevel();
	top_level = Ink_ExpressionList();
	yyin = input;
	yyparse();
	yylex_destroy();

	return;
}

void Ink_InterpreteEngine::startParse(string code)
{
	const char **input = (const char **)malloc(2 * sizeof(char *));

	input[0] = code.c_str();
	input[1] = NULL;
	input_mode = INK_STRING_INPUT;
	current_interprete_engine = this;
	cleanTopLevel();
	top_level = Ink_ExpressionList();
	
	Ink_setStringInput(input);
	yyparse();
	yylex_destroy();

	free(input);

	return;
}

Ink_Object *Ink_InterpreteEngine::execute()
{
	Ink_Object *ret;
	unsigned int i;

	for (i = 0; i < top_level.size(); i++) {
		ret = top_level[i]->eval(global_context);
		gc_engine->checkGC();
	}

	return ret;
}

void Ink_InterpreteEngine::cleanTopLevel()
{
	unsigned int i;

	for (i = 0; i < top_level.size(); i++) {
		delete top_level[i];
	}
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