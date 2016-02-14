#ifndef _ENGINE_H_
#define _ENGINE_H_

#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <stdio.h>
#include <string.h>
#include "setting.h"
#include "../general.h"
#include "../debug.h"
#include "../protocol.h"
#include "../object.h"
#include "../error.h"
#include "../context.h"
#include "../thread/actor.h"
#include "../thread/thread.h"
#include "../package/load.h"

extern FILE *yyin;
int yyparse();
int yylex_destroy();

namespace ink {

using namespace std;

class Ink_Object;
class Ink_Expression;
class Ink_InterpreteEngine;
class Ink_ContextObject;
class Ink_ContextChain;
class IGC_CollectEngine;

typedef vector<Ink_Expression *> Ink_ExpressionList;
typedef void (*Ink_EngineDestructFunction)(Ink_InterpreteEngine *engine, void *);
class Ink_EngineDestructor {
public:
	Ink_EngineDestructFunction destruct_func;
	void *arg;

	Ink_EngineDestructor(Ink_EngineDestructFunction destruct_func, void *arg)
	: destruct_func(destruct_func), arg(arg)
	{ }
};
typedef vector<Ink_EngineDestructor> Ink_CustomDestructorQueue;
typedef void *Ink_CustomEngineCom;
typedef map<InkMod_ModuleID, Ink_CustomEngineCom> Ink_CustomEngineComMap;
typedef vector<string *> Ink_CustomInterruptSignal;
typedef vector<Ink_Object *> Ink_PardonList;
typedef vector<InkCoro_Scheduler *> Ink_SchedulerStack;

extern pthread_mutex_t ink_native_exp_list_lock;
extern Ink_ExpressionList ink_native_exp_list;
void Ink_GlobalMethodInit(Ink_InterpreteEngine *engine, Ink_ContextChain *context);
void Ink_setStringInput(const char **source);

void Ink_initNativeExpression();
void Ink_cleanNativeExpression();
void Ink_insertNativeExpression(Ink_ExpressionList::iterator begin,
								Ink_ExpressionList::iterator end);
void Ink_addNativeExpression(Ink_Expression *expr);

enum Ink_InputMode {
	INK_FILE_INPUT,
	INK_STRING_INPUT
};

enum Ink_ErrorMode {
	INK_ERRMODE_DEFAULT,
	INK_ERRMODE_STRICT
};

extern DBG_FixedTypeMapping dbg_fixed_type_mapping[];

class Ink_InterpreteEngine {
public:
	Ink_ExpressionList top_level;
	IGC_CollectEngine *gc_engine;
	Ink_ContextChain *global_context;

	Ink_ErrorMode error_mode;
	Ink_InputMode input_mode;
	const char *input_file_path;

	const char *current_file_name;
	Ink_LineNoType current_line_number;

	Ink_ContextChain *trace;

	IGC_ObjectCountType igc_object_count;
	IGC_ObjectCountType igc_collect_treshold;
	IGC_CollectEngine *current_gc_engine;
	IGC_MarkType igc_mark_period;
	IGC_BondingList igc_bonding_list;
	Ink_Object *igc_global_ret_val;
	Ink_PardonList igc_pardon_list;

	Ink_InterruptSignal interrupt_signal;
	Ink_Object *interrupt_value;

	IGC_CollectEngine *coro_tmp_engine;
	Ink_SchedulerStack coro_scheduler_stack;

	pthread_mutex_t thread_pool_lock;
	ThreadIDMapStack thread_id_map_stack;
	ThreadPool thread_pool;

	vector<DBG_TypeMapping *> dbg_type_mapping;
	vector<Ink_Object *> dbg_traced_stack;

	Ink_ProtocolMap protocol_map;

	pthread_mutex_t message_mutex;
	Ink_ActorMessageQueue message_queue;

	vector<Ink_Object *> deep_clone_traced_stack;
	vector<Ink_Object *> prototype_traced_stack;

	Ink_CustomInterruptSignal custom_interrupt_signal;

	Ink_CustomDestructorQueue custom_destructor_queue;
	Ink_CustomEngineComMap custom_engine_com_map;

	Ink_InterpreteEngine();

	Ink_ContextChain *addTrace(Ink_ContextObject *context);
	void removeLastTrace();
	void removeTrace(Ink_ContextObject *context);

	inline Ink_Object *getTypePrototype(Ink_TypeTag type)
	{
		if (type < dbg_type_mapping.size()) {
			return dbg_type_mapping[type]->proto;
		}
		return NULL;
	}

	inline bool setTypePrototype(Ink_TypeTag type, Ink_Object *proto)
	{
		if (type < dbg_type_mapping.size()) {
			dbg_type_mapping[type]->proto = proto;
			return true;
		}
		return false;
	}

	inline InkCoro_Scheduler *newScheduler()
	{
		InkCoro_Scheduler *ret = new InkCoro_Scheduler();
		coro_scheduler_stack.push_back(ret);
		return ret;
	}

	inline void popScheduler()
	{
		Ink_SchedulerStack::size_type size;
		if ((size = coro_scheduler_stack.size()) > 0) {
			delete currentScheduler();
			coro_scheduler_stack.erase(coro_scheduler_stack.begin()
									   + (size - 1));
		}
		return;
	}

	inline InkCoro_Scheduler *currentScheduler()
	{
		if (!coro_scheduler_stack.size()) {
			return NULL;
		}
		return coro_scheduler_stack.back();
	}

	inline void addPardonObject(Ink_Object *obj)
	{
		igc_pardon_list.push_back(obj);
		return;
	}

	inline Ink_Object *removePardonObject(Ink_Object *obj)
	{
		Ink_PardonList::iterator pardon_iter = find(igc_pardon_list.begin(),
													igc_pardon_list.end(), obj);
		if (pardon_iter != igc_pardon_list.end()) {
			igc_pardon_list.erase(pardon_iter);
			return obj;
		}

		return NULL;
	}

	inline Ink_InterruptSignal addCustomInterruptSignal(string id)
	{
		custom_interrupt_signal.push_back(new string(id));
		return custom_interrupt_signal.size() + INTER_LAST;
	}

	inline Ink_InterruptSignal getCustomInterruptSignal(string id)
	{
		Ink_CustomInterruptSignal::iterator sig_iter;
		for (sig_iter = custom_interrupt_signal.begin();
			 sig_iter != custom_interrupt_signal.end(); sig_iter++) {
			if (id == **sig_iter)
				return sig_iter - custom_interrupt_signal.begin() + 1 + INTER_LAST;
		}
		return 0;
	}

	inline string *getCustomInterruptSignalName(Ink_InterruptSignal sig)
	{
		sig -= INTER_LAST + 1;
		if (sig < custom_interrupt_signal.size()) {
			return custom_interrupt_signal[sig];
		}
		return NULL;
	}

	inline bool deleteCustomInterruptSignal(string id)
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

	inline void disposeCustomInterruptSignal()
	{
		Ink_CustomInterruptSignal::iterator sig_iter;
		for (sig_iter = custom_interrupt_signal.begin();
			 sig_iter != custom_interrupt_signal.end(); sig_iter++) {
			delete *sig_iter;
		}
		return;
	}

	inline void setErrorMode(Ink_ErrorMode mode)
	{
		error_mode = mode;
		return;
	}

	inline Ink_ErrorMode getErrorMode()
	{
		return error_mode;
	}

	inline Ink_InterruptSignal getSignal()
	{
		return interrupt_signal;
	}

	inline void setSignal(Ink_InterruptSignal sig)
	{
		interrupt_signal = sig;
		return;
	}

	inline Ink_Object *getInterruptValue()
	{
		return interrupt_value;
	}

	inline void setInterruptValue(Ink_Object *val)
	{
		interrupt_value = val;
		return;
	}

	inline void setInterrupt(Ink_InterruptSignal sig, Ink_Object *val)
	{
		interrupt_signal = sig;
		interrupt_value = val;
		return;
	}

	inline Ink_Object *trapSignal()
	{
		Ink_Object *tmp = interrupt_value;
		interrupt_signal = INTER_NONE;
		interrupt_value = NULL;
		return tmp;
	}

	inline void setGlobalReturnValue(Ink_Object *ret_val)
	{
		igc_global_ret_val = ret_val;
		return;
	}

	inline Ink_Object *getGlobalReturnValue()
	{
		return igc_global_ret_val;
	}

	inline void initGCCollect()
	{
		igc_bonding_list = IGC_BondingList();
		return;
	}

	inline void addGCBonding(Ink_HashTable *from, Ink_HashTable *to)
	{
		igc_bonding_list.push_back(IGC_Bonding(from, to));
		return;
	}

	inline IGC_Bonding searchGCBonding(Ink_HashTable *to)
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

	void breakUnreachableBonding(Ink_HashTable *to);

	inline int addEngineCom(InkMod_ModuleID id, Ink_CustomEngineCom com)
	{
		if (custom_engine_com_map.find(id) != custom_engine_com_map.end()) {
			return 1; // com exist
		}

		custom_engine_com_map[id] = com;
		return 0;
	}

	template <typename AS_TYPE>
	inline AS_TYPE *getEngineComAs(InkMod_ModuleID id)
	{
		if (custom_engine_com_map.find(id) == custom_engine_com_map.end()) {
			return NULL;
		}

		return (AS_TYPE *)custom_engine_com_map[id];
	}

	inline void addDestructor(Ink_EngineDestructor engine_destructor)
	{
		custom_destructor_queue.push_back(engine_destructor);
		return;
	}

	inline void callAllDestructor()
	{
		Ink_CustomDestructorQueue::size_type i;
		for (i = 0; i < custom_destructor_queue.size(); i++) {
			custom_destructor_queue[i].destruct_func(this, custom_destructor_queue[i].arg);
		}
		return;
	}

	inline void initPrototypeSearch()
	{
		prototype_traced_stack = vector<Ink_Object *>();
		return;
	}

	inline bool prototypeHasTraced(Ink_Object *obj)
	{
		return find(prototype_traced_stack.begin(),
					prototype_traced_stack.end(), obj)
			   != prototype_traced_stack.end();
	}

	inline void addPrototypeTrace(Ink_Object *obj)
	{
		prototype_traced_stack.push_back(obj);
		return;
	}

	inline void initDeepClone()
	{
		deep_clone_traced_stack = vector<Ink_Object *>();
		return;
	}

	inline bool cloneDeepHasTraced(Ink_Object *obj)
	{
		return find(deep_clone_traced_stack.begin(),
					deep_clone_traced_stack.end(), obj)
			   != deep_clone_traced_stack.end();
	}

	inline void addDeepCloneTrace(Ink_Object *obj)
	{
		deep_clone_traced_stack.push_back(obj);
		return;
	}

	inline Ink_Object *receiveMessage()
	{
		Ink_Object *ret = NULL;
		pthread_mutex_lock(&message_mutex);
		if (!message_queue.empty()) {
			Ink_ActorMessage msg = message_queue.front();
			ret = new Ink_Object(this);
			ret->setSlot("msg", new Ink_String(this, msg.msg));
			ret->setSlot("sender", new Ink_String(this, msg.sender));
			message_queue.pop();
		}
		pthread_mutex_unlock(&message_mutex);
		return ret;
	}

	inline void sendInMessage(Ink_InterpreteEngine *sender, string msg)
	{
		pthread_mutex_lock(&message_mutex);
		message_queue.push(Ink_ActorMessage(new string(msg), InkActor_getActorName(sender)));
		pthread_mutex_unlock(&message_mutex);
		return;
	}

	inline void disposeAllMessage()
	{
		pthread_mutex_lock(&message_mutex);
		while (!message_queue.empty()) {
			if (message_queue.front().msg)
				delete message_queue.front().msg;
			if (message_queue.front().sender)
				delete message_queue.front().sender;
			message_queue.pop();
		}
		pthread_mutex_unlock(&message_mutex);
		return;
	}

	inline void addProtocol(const char *name, Ink_Protocol proto)
	{
		protocol_map[string(name)] = proto;
		return;
	}

	inline Ink_Protocol findProtocol(const char *name)
	{
		if (protocol_map.find(string(name)) != protocol_map.end()) {
			return protocol_map[string(name)];
		}
		return NULL;
	}

	inline int initThread()
	{
		//thread_lock.init();
		pthread_mutex_init(&thread_pool_lock, NULL);
		return 0;
	}

#define CURRENT_LAYER (thread_id_map_stack.size() - 1)

	inline ThreadID getThreadID()
	{
		ThreadID id;

		pthread_mutex_lock(&thread_pool_lock);
		id = thread_id_map_stack[CURRENT_LAYER][getThreadID_raw()];
		pthread_mutex_unlock(&thread_pool_lock);

		return id;
	}

	inline ThreadID registerThread(ThreadID id)
	{
		pthread_mutex_lock(&thread_pool_lock);
		thread_id_map_stack[CURRENT_LAYER][getThreadID_raw()] = id;
		pthread_mutex_unlock(&thread_pool_lock);

		return id;
	}

	inline void addLayer()
	{
		pthread_mutex_lock(&thread_pool_lock);
		thread_id_map_stack.push_back(ThreadIDMap());
		pthread_mutex_unlock(&thread_pool_lock);
	}

	inline void removeLayer()
	{
		pthread_mutex_lock(&thread_pool_lock);
		thread_id_map_stack.pop_back();
		pthread_mutex_unlock(&thread_pool_lock);
	}

	inline ThreadLayerType getCurrentLayer()
	{
		pthread_mutex_lock(&thread_pool_lock);
		ThreadLayerType ret = CURRENT_LAYER;
		pthread_mutex_unlock(&thread_pool_lock);
		return ret;
	}

	inline void addThread(pthread_t *thread)
	{
		pthread_mutex_lock(&thread_pool_lock);
		thread_pool.push_back(thread);
		pthread_mutex_unlock(&thread_pool_lock);

		return;
	}

	inline void joinAllThread()
	{
		pthread_t *thd;
		ThreadPool::size_type i;
		for (i = 0; i < thread_pool.size(); i++) {
			pthread_mutex_lock(&thread_pool_lock);
			thd = thread_pool[i];
			pthread_mutex_unlock(&thread_pool_lock);

			pthread_join(*thd, NULL);
			free(thd);
		}
		return;
	}

	inline Ink_ContextChain *getTrace()
	{ return trace; }

	inline int setCurrentGC(IGC_CollectEngine *engine)
	{
		current_gc_engine = engine;
		return 0;
	}

	inline IGC_CollectEngine *getCurrentGC()
	{
		return current_gc_engine;
	}

	inline void setFilePath(const char *path)
	{
		input_file_path = path;
		return;
	}

	inline const char *getFilePath()
	{
		return  input_file_path;
	}

	inline void initTypeMapping()
	{
		Ink_TypeTag i;

		dbg_type_mapping = vector<DBG_TypeMapping *>();

		for (i = 0; i < INK_LAST; i++) {
			dbg_type_mapping.push_back(new DBG_TypeMapping(i, dbg_fixed_type_mapping[i].name));
		}

		return;
	}

	inline void disposeTypeMapping()
	{
		vector<DBG_TypeMapping *>::iterator type_iter;

		for (type_iter = dbg_type_mapping.begin();
			 type_iter != dbg_type_mapping.end(); type_iter++) {
			delete *type_iter;
		}
		dbg_type_mapping = vector<DBG_TypeMapping *>();

		return;
	}

	inline Ink_TypeTag registerType(const char *name)
	{
		string *tmp_str = new string(name);
		Ink_TypeTag ret = dbg_type_mapping.size();
		dbg_type_mapping.push_back(new DBG_TypeMapping(ret, tmp_str->c_str(), tmp_str));
		return ret;
	}

	inline const char *getTypeName(Ink_TypeTag type_tag)
	{
		return dbg_type_mapping[type_tag]->name;
	}

	inline void initPrintDebugInfo()
	{
		dbg_traced_stack = vector<Ink_Object *>();
		return;
	}
	
	void printSlotInfo(FILE *fp, Ink_Object *obj, string prefix = "");
	void printDebugInfo(FILE *fp, Ink_Object *obj, std::string prefix = DBG_DEFAULT_PREFIX,
						std::string slot_prefix = "", bool if_scan_slot = true);
	inline void printDebugInfo(bool if_scan_slot, FILE *fp, Ink_Object *obj,
							   std::string prefix = DBG_DEFAULT_PREFIX, std::string slot_prefix = "")
	{
		printDebugInfo(fp, obj, prefix, slot_prefix, if_scan_slot);
		return;
	}
	void printTrace(FILE *fp, Ink_ContextChain *context, string prefix = DBG_DEFAULT_PREFIX);

	void startParse(Ink_InputSetting setting);
	void startParse(FILE *input = stdin, bool close_fp = false);
	void startParse(string code);
	Ink_Object *execute(Ink_ContextChain *context = NULL, bool if_trap_signal = true);
	Ink_Object *execute(Ink_Expression *exp);
	static void cleanExpressionList(Ink_ExpressionList exp_list);
	static void cleanContext(Ink_ContextChain *context);

	~Ink_InterpreteEngine();
};

}

#endif
