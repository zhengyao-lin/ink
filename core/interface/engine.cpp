#include "engine.h"
#include "core/gc/collect.h"
#include "core/hash.h"
#include "core/object.h"
#include "core/expression.h"
#include "core/general.h"
#include "core/thread/thread.h"

static Ink_InterpreteEngine *ink_parse_engine = NULL;

pthread_mutex_t ink_parse_lock = PTHREAD_MUTEX_INITIALIZER;

Ink_InterpreteEngine *Ink_getParseEngine()
{
	return ink_parse_engine;
}

void Ink_setParseEngine(Ink_InterpreteEngine *engine)
{
	ink_parse_engine = engine;
	return;
}

Ink_InterpreteEngine::Ink_InterpreteEngine()
{
	// gc_lock.init();

	igc_object_count = 0;
	igc_collect_treshold = IGC_COLLECT_TRESHOLD;
	igc_mark_period = 1;
	input_file_path = NULL;
	current_line_number = -1;
	trace = NULL;
	CGC_interrupt_signal = INTER_NONE;
	CGC_interrupt_value = NULL;
	native_exp_list = Ink_ExpressionList();
	tmp_prog_path = NULL;
	ink_sync_call_tmp_engine = NULL;
	pthread_mutex_init(&ink_sync_call_mutex, NULL);
	ink_sync_call_max_thread = 0;
	ink_sync_call_current_thread = -1;
	ink_sync_call_end_flag = vector<bool>();
	string_pool = vector<string *>();
	dbg_type_mapping_length = 0;
	dbg_type_mapping = NULL;
	protocol_map = Ink_ProtocolMap();
	pthread_mutex_init(&message_mutex, NULL);
	message_queue = Ink_ActorMessageQueue();

	initThread();
	initTypeMapping();
	initPrintDebugInfo();

	gc_engine = new IGC_CollectEngine(this);
	setCurrentGC(gc_engine);
	global_context = new Ink_ContextChain(new Ink_ContextObject(this));
	// gc_engine->initContext(global_context);

	global_context->context->setSlot("this", global_context->context);
	Ink_GlobalMethodInit(this, global_context);

	global_context->context->setDebugName("__global_context__");
	addTrace(global_context->context)->setDebug(-1, global_context->context);
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

void Ink_InterpreteEngine::removeTrace(Ink_ContextObject *context)
{
	Ink_ContextChain *i;

	for (i = trace->getLocal(); i && i->context != context; i = i->outer) ;
	if (i) {
		if (i->inner) {
			i->inner->outer = i->outer;
		}
		if (i->outer) {
			i->outer->inner = i->inner;
		}
		delete i;
	}
	return;
}

void Ink_InterpreteEngine::startParse(Ink_InputSetting setting)
{
	pthread_mutex_lock(&ink_parse_lock);
	Ink_InterpreteEngine *backup = Ink_getParseEngine();
	Ink_setParseEngine(this);
	
	setFilePath(setting.getFilePath());

	input_mode = INK_FILE_INPUT;
	// CGC_code_mode = setting.getMode();
	// cleanTopLevel();
	top_level = Ink_ExpressionList();
	yyin = setting.getInput();
	yyparse();
	yylex_destroy();

	setting.clean();

	Ink_setParseEngine(backup);
	pthread_mutex_unlock(&ink_parse_lock);

	return;
}

void Ink_InterpreteEngine::startParse(FILE *input, bool close_fp)
{
	pthread_mutex_lock(&ink_parse_lock);
	Ink_InterpreteEngine *backup = Ink_getParseEngine();
	Ink_setParseEngine(this);
	
	input_mode = INK_FILE_INPUT;
	// cleanTopLevel();
	top_level = Ink_ExpressionList();
	yyin = input;
	yyparse();
	yylex_destroy();

	if (close_fp) fclose(input);

	Ink_setParseEngine(backup);
	pthread_mutex_unlock(&ink_parse_lock);

	return;
}

void Ink_InterpreteEngine::startParse(string code)
{
	pthread_mutex_lock(&ink_parse_lock);
	Ink_InterpreteEngine *backup = Ink_getParseEngine();
	Ink_setParseEngine(this);

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

	Ink_setParseEngine(backup);
	pthread_mutex_unlock(&ink_parse_lock);

	return;
}

Ink_Object *Ink_InterpreteEngine::execute(Ink_ContextChain *context)
{
	char *current_dir = NULL, *redirect = NULL;
	Ink_InterpreteEngine *engine = this;

	if (getFilePath()) {
		current_dir = getCurrentDir();
		redirect = getBasePath(getFilePath());
		if (redirect) {
			changeDir(redirect);
			free(redirect);
		}
	}

	Ink_Object *ret = NULL_OBJ;
	unsigned int i;

	if (!context) context = global_context;
	for (i = 0; i < top_level.size(); i++) {
		getCurrentGC()->checkGC();
		ret = top_level[i]->eval(this, context);
		if (CGC_interrupt_signal != INTER_NONE) {
			trapSignal(this); // trap all
			break;
		}
	}

	if (current_dir) {
		changeDir(current_dir);
		free(current_dir);
	}

	return ret;
}

Ink_Object *Ink_InterpreteEngine::execute(Ink_Expression *exp)
{
	Ink_Object *ret;
	Ink_ContextChain *context = NULL;

	if (!context) context = global_context;
	getCurrentGC()->checkGC();
	ret = exp->eval(this, context);

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
	gc_engine->collectGarbage(true);
	delete gc_engine;

	unsigned int i;
	for (i = 0; i < native_exp_list.size(); i++) {
		delete native_exp_list[i];
	}

	cleanExpressionList(top_level);
	cleanContext(global_context);
	cleanContext(trace);
	
	disposeTypeMapping();
	disposeStringPool();

	if (tmp_prog_path)
		free(tmp_prog_path);
}
