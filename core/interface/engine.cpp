#include "engine.h"
#include "core/hash.h"
#include "core/object.h"
#include "core/expression.h"
#include "core/general.h"
#include "core/syntax/syntax.h"
#include "core/native/native.h"
#include "core/thread/thread.h"
#include "core/gc/collect.h"

namespace ink {
	
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
	Ink_Object *tmp, *obj_proto;
	Ink_ContextObject *global;

	igc_collect_threshold = igc_collect_threshold_unit = IGC_COLLECT_THRESHOLD_UNIT;
	// igc_mark_period = 1;
	igc_global_ret_val = NULL;
	igc_pardon_list = Ink_PardonList();
	igc_grey_list = IGC_GreyList();

	error_mode = INK_ERRMODE_DEFAULT;

	input_file_path = NULL;
	current_file_name = NULL;
	current_line_number = -1;
	
	trace = NULL;
	
	interrupt_signal = INTER_NONE;
	interrupt_value = NULL;

	coro_tmp_engine = NULL;
	coro_scheduler_stack = Ink_SchedulerStack();

	dbg_print_detail = false;
	dbg_max_trace = DBG_DEFAULT_MAX_TRACE;
	
	protocol_map = Ink_ProtocolMap();
	pthread_mutex_init(&message_lock, NULL);
	message_queue = Ink_ActorMessageQueue();

	pthread_mutex_init(&watcher_lock, NULL);
	watcher_list = Ink_ActorWatcherList();
	
	custom_interrupt_signal = Ink_CustomInterruptSignal();

	custom_destructor_queue = Ink_CustomDestructorQueue();
	custom_engine_com_map = Ink_CustomEngineComMap();

	initThread();
	initTypeMapping();
	initPrintDebugInfo();
	initPrototypeSearch();
	initGCCollect();

	const_table = Ink_ConstantTable();

	gc_engine = new IGC_CollectEngine(this);
	setCurrentGC(gc_engine);
	global_context = new Ink_ContextChain(new Ink_ContextObject(this));
	global = global_context->getGlobal();
	// gc_engine->initContext(global_context);

	global->setSlot_c("$object", obj_proto = new Ink_Object(this));
	setTypePrototype(INK_OBJECT, obj_proto);
	global->setProto(obj_proto);

	global->setSlot_c("$function", tmp = new Ink_FunctionObject(this));
	setTypePrototype(INK_FUNCTION, tmp);
	tmp->setProto(obj_proto);
	tmp->derivedMethodInit(this);
	obj_proto->derivedMethodInit(this);

	global->setSlot_c("$exp_list", tmp = new Ink_ExpListObject(this));
	setTypePrototype(INK_EXPLIST, tmp);
	tmp->setProto(obj_proto);
	tmp->derivedMethodInit(this);

	global->setSlot_c("$numeric", tmp = new Ink_Numeric(this, Ink_NumericValue()));
	setTypePrototype(INK_NUMERIC, tmp);
	tmp->setProto(obj_proto);
	tmp->derivedMethodInit(this);

	global->setSlot_c("$string", tmp = new Ink_String(this, ""));
	setTypePrototype(INK_STRING, tmp);
	tmp->setProto(obj_proto);
	tmp->derivedMethodInit(this);

	global->setSlot_c("$array", tmp = new Ink_Array(this));
	setTypePrototype(INK_ARRAY, tmp);
	tmp->setProto(obj_proto);
	tmp->derivedMethodInit(this);

	global->setSlot_c("self", global);
	global->setSlot_c("top", global);
	global->setSlot_c("let", global);
	global->setSlot_c("auto", tmp = new Ink_Object(this));
	tmp->setSlot_c("missing", new Ink_FunctionObject(this, InkNative_Auto_Missing_i));
	// global->setSlot_c("fix", tmp = new Ink_Object(this));
	// tmp->setSlot_c("missing", new Ink_FunctionObject(this, InkNative_Fix_Missing_i));
	Ink_GlobalMethodInit(this, global_context);

	global->setDebugName("__global_context__");
	addTrace(global)->setDebug("<root engine>", -1, global);
}

Ink_ContextChain_sub *Ink_InterpreteEngine::addTrace(Ink_ContextObject *context)
{
	if (!trace) return (trace = new Ink_ContextChain(context))->head;
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
	trace->removeContext(context);
	return;
}

inline void setArgv(Ink_InterpreteEngine *engine, vector<char *> argv)
{
	unsigned int i;
	Ink_Array *val = new Ink_Array(engine);

	for (i = 0; i < argv.size(); i++) {
		val->value.push_back(new Ink_HashTable(new Ink_String(engine, string(argv[i])), val));
	}

	engine->global_context->getGlobal()->setSlot(INK_ARGV_NAME, val);
	
	return;
}

void Ink_InterpreteEngine::startParse(Ink_InputSetting setting)
{
	InkParser_lockParseLock();
	Ink_InterpreteEngine *backup = InkParser_getParseEngine();
	InkParser_setParseEngine(this);
	
	setFilePath(setting.getFilePath());

	input_mode = INK_FILE_INPUT;
	// CGC_code_mode = setting.getMode();
	// cleanTopLevel();
	top_level = Ink_ExpressionList();
	yyin = setting.getInput();
	yyparse();
	yylex_destroy();

	setting.clean();

	InkParser_setParseEngine(backup);
	InkParser_unlockParseLock();
	setArgv(this, setting.getArgument());

	return;
}

void Ink_InterpreteEngine::startParse(FILE *input, bool close_fp)
{
	InkParser_lockParseLock();
	Ink_InterpreteEngine *backup = InkParser_getParseEngine();
	InkParser_setParseEngine(this);
	
	input_mode = INK_FILE_INPUT;
	// cleanTopLevel();
	top_level = Ink_ExpressionList();
	yyin = input;
	yyparse();
	yylex_destroy();

	if (close_fp) fclose(input);

	InkParser_setParseEngine(backup);
	InkParser_unlockParseLock();

	return;
}

void Ink_InterpreteEngine::startParse(string code)
{
	InkParser_lockParseLock();
	Ink_InterpreteEngine *backup = InkParser_getParseEngine();
	InkParser_setParseEngine(this);

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

	InkParser_setParseEngine(backup);
	InkParser_unlockParseLock();

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
	string *tmp_str = NULL;
	Ink_ExceptionRaw *tmp_ex = NULL;

	if (!context) context = global_context;
	local = context->getLocal();
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
					tmp_sig_name = "<unregistered>";
				}
			}

			if (getSignal() != INTER_EXIT)
				InkWarn_Trapping_Untrapped_Signal(engine, tmp_sig_name);

			ret = getInterruptValue();

			if (getSignal() == INTER_THROW) {
				if (!broadcastWatcher("error exit", tmp_ex = Ink_ExceptionRaw::toRaw(ret))) {
					delete tmp_ex;
				}
			}

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
	delete context;
	return;
}

// from engine inline

Ink_InterruptSignal Ink_InterpreteEngine::getCustomInterruptSignal(string id)
{
	Ink_CustomInterruptSignal::iterator sig_iter;
	for (sig_iter = custom_interrupt_signal.begin();
		 sig_iter != custom_interrupt_signal.end(); sig_iter++) {
		if (id == **sig_iter)
			return sig_iter - custom_interrupt_signal.begin() + 1 + INTER_LAST;
	}
	return 0;
}

bool Ink_InterpreteEngine::deleteCustomInterruptSignal(string id)
{
	Ink_CustomInterruptSignal::iterator sig_iter;
	for (sig_iter = custom_interrupt_signal.begin();
		 sig_iter != custom_interrupt_signal.end(); sig_iter++) {
		if (id == **sig_iter) {
			delete *sig_iter;
			custom_interrupt_signal.erase(sig_iter);
			return true;
		}
	}
	return false;
}

void Ink_InterpreteEngine::disposeCustomInterruptSignal()
{
	Ink_CustomInterruptSignal::iterator sig_iter;
	for (sig_iter = custom_interrupt_signal.begin();
		 sig_iter != custom_interrupt_signal.end(); sig_iter++) {
		delete *sig_iter;
	}
	return;
}

void Ink_InterpreteEngine::breakUnreachableBonding(Ink_HashTable *to)
{
	IGC_BondingList::iterator bond_iter;
	for (bond_iter = igc_bonding_list.begin();
		 bond_iter != igc_bonding_list.end(); bond_iter++) {
		if ((*bond_iter).second == to) {
			InkWarn_Unreachable_Bonding(this);
			(*bond_iter).first->bonding = NULL;
			getCurrentGC()->doMark(getInterruptValue());
		}
	}
	return;
}

IGC_Bonding Ink_InterpreteEngine::searchGCBonding(Ink_HashTable *to)
{
	IGC_BondingList::iterator bond_iter;
	for (bond_iter = igc_bonding_list.begin();
		 bond_iter != igc_bonding_list.end(); bond_iter++) {
		if ((*bond_iter).second == to) {
			return *bond_iter;
		}
	}
	return IGC_Bonding(NULL, NULL);
}

void Ink_InterpreteEngine::callAllDestructor()
{
	Ink_CustomDestructorQueue::size_type i;
	for (i = 0; i < custom_destructor_queue.size(); i++) {
		custom_destructor_queue[i].destruct_func(this, custom_destructor_queue[i].arg);
	}
	return;
}

Ink_InterpreteEngine::~Ink_InterpreteEngine()
{
	callAllDestructor();
	disposeAllMessage();

	gc_engine->collectGarbage(true);
	delete gc_engine;

	disposeConstant();

	cleanExpressionList(top_level);
	cleanContext(global_context);
	cleanContext(trace);
	
	disposeTypeMapping();
	disposeCustomInterruptSignal();
}

}
