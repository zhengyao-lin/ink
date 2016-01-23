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
#include "../thread/actor.h"
#include "../thread/thread.h"
#include "../package/load.h"

using namespace std;

class Ink_Object;
class Ink_Expression;
class Ink_InterpreteEngine;
class IGC_CollectEngine;
class Ink_ContextObject;
class Ink_ContextChain;

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

extern FILE *yyin;
extern pthread_mutex_t ink_native_exp_list_lock;
extern Ink_ExpressionList ink_native_exp_list;
int yyparse();
int yylex_destroy();
void Ink_GlobalMethodInit(Ink_InterpreteEngine *engine, Ink_ContextChain *context);
void Ink_setStringInput(const char **source);

void Ink_initNativeExpression();
void Ink_cleanNativeExpression();
void Ink_insertNativeExpression(Ink_ExpressionList::iterator begin,
								Ink_ExpressionList::iterator end);
void Ink_addNativeExpression(Ink_Expression *expr);

Ink_InterpreteEngine *Ink_getParseEngine();
void Ink_setParseEngine(Ink_InterpreteEngine *engine);

enum Ink_InputMode {
	INK_FILE_INPUT,
	INK_STRING_INPUT
};

extern DBG_FixedTypeMapping dbg_fixed_type_mapping[];

class Ink_InterpreteEngine {
public:
	Ink_ExpressionList top_level;
	IGC_CollectEngine *gc_engine;
	Ink_ContextChain *global_context;

	Ink_InputMode input_mode;
	const char *input_file_path;
	Ink_LineNoType current_line_number;

	Ink_ContextChain *trace;

	// MutexLock gc_lock;
	IGC_ObjectCountType igc_object_count;
	IGC_ObjectCountType igc_collect_treshold;
	IGC_CollectEngine *current_gc_engine;
	IGC_MarkType igc_mark_period;
	// std::map<int, IGC_CollectEngine *> gc_engine_map;

	InterruptSignal CGC_interrupt_signal;
	Ink_Object *CGC_interrupt_value;

	char *tmp_prog_path;

	IGC_CollectEngine *ink_sync_call_tmp_engine;
	pthread_mutex_t ink_sync_call_mutex;
	ThreadID ink_sync_call_max_thread;
	ThreadID ink_sync_call_current_thread;
	vector<bool> ink_sync_call_end_flag;

	MutexLock thread_lock;
	ThreadIDMapStack thread_id_map_stack;
	ThreadPool thread_pool;

	vector<string *> string_pool;

	DBG_CustomTypeType dbg_type_mapping_length;
	DBG_TypeMapping *dbg_type_mapping;
	vector<Ink_Object *> dbg_traced_stack;

	Ink_ProtocolMap protocol_map;

	pthread_mutex_t message_mutex;
	Ink_ActorMessageQueue message_queue;

	vector<Ink_Object *> deep_clone_traced_stack;
	vector<Ink_Object *> prototype_traced_stack;

	Ink_CustomDestructorQueue custom_destructor_queue;
	Ink_CustomEngineComMap custom_engine_com_map;

	Ink_InterpreteEngine();

	Ink_ContextChain *addTrace(Ink_ContextObject *context);
	void removeLastTrace();
	void removeTrace(Ink_ContextObject *context);

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
		thread_lock.init();
		return 0;
	}

#define CURRENT_LAYER (thread_id_map_stack.size() - 1)

	inline ThreadID getThreadID()
	{
		ThreadID id;

		thread_lock.lock();
		id = thread_id_map_stack[CURRENT_LAYER][getThreadID_raw()];
		thread_lock.unlock();

		return id;
	}

	inline ThreadID registerThread(ThreadID id)
	{
		thread_lock.lock();
		thread_id_map_stack[CURRENT_LAYER][getThreadID_raw()] = id;
		thread_lock.unlock();

		return id;
	}

	inline void addLayer()
	{
		thread_lock.lock();
		thread_id_map_stack.push_back(ThreadIDMap());
		thread_lock.unlock();
	}

	inline void removeLayer()
	{
		thread_lock.lock();
		thread_id_map_stack.pop_back();
		thread_lock.unlock();
	}

	inline ThreadLayerType getCurrentLayer()
	{
		thread_lock.lock();
		ThreadLayerType ret = CURRENT_LAYER;
		thread_lock.unlock();
		return ret;
	}

	inline void addThread(pthread_t *thread)
	{
		thread_lock.lock();
		thread_pool.push_back(thread);
		thread_lock.unlock();

		return;
	}

	inline void joinAllThread()
	{
		pthread_t *thd;
		ThreadPool::size_type i;
		//thread_lock.lock();
		for (i = 0; i < thread_pool.size(); i++) {
			thread_lock.lock();
			thd = thread_pool[i];
			thread_lock.unlock();

			pthread_join(*thd, NULL);
			free(thd);
		}
		//thread_lock.unlock();
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

	inline string *addToStringPool(const char *str)
	{
		string *tmp;
		string_pool.push_back(tmp = new string(str ? str : ""));
		return tmp;
	}

	inline string *addToStringPool(string *str)
	{
		string_pool.push_back(str);
		return str;
	}

	inline void disposeStringPool()
	{
		vector<string *>::size_type i;
		for (i = 0; i < string_pool.size(); i++) {
			delete string_pool[i];
		}
		string_pool = vector<string *>();
		return;
	}

	inline void initTypeMapping()
	{
		DBG_CustomTypeType i;

		dbg_type_mapping_length = INK_LAST;
		dbg_type_mapping = (DBG_TypeMapping *)malloc(sizeof(DBG_TypeMapping) * dbg_type_mapping_length);

		for (i = 0; i < INK_LAST; i++) {
			dbg_type_mapping[i] = DBG_TypeMapping(i, dbg_fixed_type_mapping[i].name);
		}

		return;
	}

	inline void disposeTypeMapping()
	{
		free(dbg_type_mapping);
		return;
	}

	inline DBG_CustomTypeType registerType(const char *name)
	{
		DBG_CustomTypeType ret = dbg_type_mapping_length++;
		dbg_type_mapping = (DBG_TypeMapping *)realloc(dbg_type_mapping,
													  sizeof(DBG_TypeMapping) * dbg_type_mapping_length);
		dbg_type_mapping[ret] = DBG_TypeMapping(ret, addToStringPool(name)->c_str());
		return ret;
	}

	inline const char *getTypeName(DBG_CustomTypeType type_tag)
	{
		return dbg_type_mapping[type_tag].friendly_name;
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
	Ink_Object *execute(Ink_ContextChain *context = NULL);
	Ink_Object *execute(Ink_Expression *exp);
	static void cleanExpressionList(Ink_ExpressionList exp_list);
	static void cleanContext(Ink_ContextChain *context);

	~Ink_InterpreteEngine();
};

#endif
