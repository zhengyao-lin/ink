#include "engine.h"
#include "core/hash.h"
#include "core/object.h"
#include "core/expression.h"
#include "core/general.h"
#include "core/native/native.h"
#include "core/thread/thread.h"
#include "core/gc/collect.h"

namespace ink {

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

pthread_mutex_t ink_native_exp_list_lock = PTHREAD_MUTEX_INITIALIZER;
Ink_ExpressionList ink_native_exp_list;

void Ink_initNativeExpression()
{
	ink_native_exp_list = Ink_ExpressionList();
	return;
}

void Ink_cleanNativeExpression()
{
	unsigned int i;

	pthread_mutex_lock(&ink_native_exp_list_lock);
	for (i = 0; i < ink_native_exp_list.size(); i++) {
		delete ink_native_exp_list[i];
	}
	pthread_mutex_unlock(&ink_native_exp_list_lock);

	return;
}

void Ink_insertNativeExpression(Ink_ExpressionList::iterator begin,
								Ink_ExpressionList::iterator end)
{
	pthread_mutex_lock(&ink_native_exp_list_lock);
	ink_native_exp_list.insert(ink_native_exp_list.end(), begin, end);
	pthread_mutex_unlock(&ink_native_exp_list_lock);
	return;
}

void Ink_addNativeExpression(Ink_Expression *expr)
{
	pthread_mutex_lock(&ink_native_exp_list_lock);
	ink_native_exp_list.push_back(expr);
	pthread_mutex_unlock(&ink_native_exp_list_lock);
	return;
}

Ink_InterpreteEngine::Ink_InterpreteEngine()
{
	// gc_lock.init();
	Ink_Object *tmp;

	igc_object_count = 0;
	igc_collect_treshold = IGC_COLLECT_TRESHOLD;
	igc_mark_period = 1;
	igc_global_ret_val = NULL;
	igc_pardon_list = Ink_PardonList();

	error_mode = INK_ERRMODE_DEFAULT;

	input_file_path = NULL;
	current_file_name = NULL;
	current_line_number = -1;
	
	trace = NULL;
	
	interrupt_signal = INTER_NONE;
	interrupt_value = NULL;

	coro_tmp_engine = NULL;
	coro_scheduler_stack = Ink_SchedulerStack();
	
	protocol_map = Ink_ProtocolMap();
	pthread_mutex_init(&message_mutex, NULL);
	message_queue = Ink_ActorMessageQueue();
	
	custom_interrupt_signal = Ink_CustomInterruptSignal();

	custom_destructor_queue = Ink_CustomDestructorQueue();
	custom_engine_com_map = Ink_CustomEngineComMap();

	initThread();
	initTypeMapping();
	initPrintDebugInfo();

	gc_engine = new IGC_CollectEngine(this);
	setCurrentGC(gc_engine);
	global_context = new Ink_ContextChain(new Ink_ContextObject(this));
	// gc_engine->initContext(global_context);

	// make sure the slot name is $<type name>
	global_context->context->setSlot("$object", tmp = new Ink_Object(this));
	tmp->derivedMethodInit(this);
	global_context->context->setSlot("$function", tmp = new Ink_FunctionObject(this));
	tmp->derivedMethodInit(this);
	global_context->context->setSlot("$numeric", tmp = new Ink_Numeric(this));
	tmp->derivedMethodInit(this);
	global_context->context->setSlot("$string", tmp = new Ink_String(this, ""));
	tmp->derivedMethodInit(this);
	global_context->context->setSlot("$array", tmp = new Ink_Array(this));
	tmp->derivedMethodInit(this);

	// global_context->context->setSlot("this", global_context->context);
	global_context->context->setSlot("top", global_context->context);
	global_context->context->setSlot("let", global_context->context);
	global_context->context->setSlot("auto", tmp = new Ink_Object(this));
	tmp->setSlot("missing", new Ink_FunctionObject(this, Ink_Auto_Missing));
	Ink_GlobalMethodInit(this, global_context);

	global_context->context->setDebugName("__global_context__");
	addTrace(global_context->context)->setDebug("<root engine>", -1, global_context->context);
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

inline void setArgv(Ink_InterpreteEngine *engine, vector<char *> argv)
{
	unsigned int i;
	Ink_ArrayValue arr_val;

	for (i = 0; i < argv.size(); i++) {
		arr_val.push_back(new Ink_HashTable(new Ink_String(engine, string(argv[i]))));
	}

	engine->global_context
	->getGlobal()->context
	->setSlot(INK_ARGV_NAME, new Ink_Array(engine, arr_val));
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
	setArgv(this, setting.getArgument());

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

Ink_Object *Ink_InterpreteEngine::execute(Ink_ContextChain *context, bool if_trap_signal)
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
	Ink_ContextObject *local = NULL;
	unsigned int i;
	const char *tmp_sig_name = NULL;
	string *tmp_str  =NULL;

	if (!context) context = global_context;
	local = context->getLocal()->context;
	for (i = 0; i < top_level.size(); i++) {
		getCurrentGC()->checkGC();
		ret = top_level[i]->eval(this, context);
		if (getSignal() != INTER_NONE) {
			/* interrupt event triggered */
			Ink_FunctionObject::triggerInterruptEvent(engine, context, local, local);

			if (getSignal() == INTER_NONE) continue;

			if (getSignal() < INTER_LAST) {
				tmp_sig_name = getNativeSignalName(interrupt_signal);
			} else {
				if ((tmp_str = getCustomInterruptSignalName(interrupt_signal)) != NULL) {
					tmp_sig_name = tmp_str->c_str();
				} else {
					tmp_sig_name = "<unregisterred>";
				}
			}
			if (getSignal() != INTER_EXIT)
				InkWarn_Trapping_Untrapped_Signal(engine, tmp_sig_name);

			ret = getInterruptValue();
			if (if_trap_signal) {
				trapSignal();
			}
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
	callAllDestructor();
	disposeAllMessage();

	gc_engine->collectGarbage(true);
	delete gc_engine;

	cleanExpressionList(top_level);
	cleanContext(global_context);
	cleanContext(trace);
	
	disposeTypeMapping();
	disposeCustomInterruptSignal();
}

}
