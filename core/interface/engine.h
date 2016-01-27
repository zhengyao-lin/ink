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

extern pthread_mutex_t ink_native_exp_list_lock;
extern Ink_ExpressionList ink_native_exp_list;
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

	const char *current_file_name;
	Ink_LineNoType current_line_number;

	Ink_ContextChain *trace;

	// MutexLock gc_lock;
	IGC_ObjectCountType igc_object_count;
	IGC_ObjectCountType igc_collect_treshold;
	IGC_CollectEngine *current_gc_engine;
	IGC_MarkType igc_mark_period;
	IGC_BondingList igc_bonding_list;
	Ink_Object *igc_global_ret_val;
	// std::map<int, IGC_CollectEngine *> gc_engine_map;

	InterruptSignal CGC_interrupt_signal;
	Ink_Object *CGC_interrupt_value;

	char *tmp_prog_path;

	IGC_CollectEngine *ink_sync_call_tmp_engine;
	pthread_mutex_t ink_sync_call_mutex;
	ThreadID ink_sync_call_max_thread;
	ThreadID ink_sync_call_current_thread;
	vector<bool> ink_sync_call_end_flag;

	pthread_mutex_t thread_pool_lock;
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

	inline void breakUnreachableBonding(Ink_HashTable *to)
	{
		IGC_BondingList::iterator bond_iter;
		for (bond_iter = igc_bonding_list.begin();
			 bond_iter != igc_bonding_list.end(); bond_iter++) {
			if ((*bond_iter).second == to) {
				InkWarn_Unreachable_Bonding(this);
				(*bond_iter).first->bonding = NULL;
			}
		}
		return;
	}

	inline Ink_HashTable *searchNativeMethod(Ink_TypeTag type, const char *name)
	{
		Ink_Object *tmp = NULL;
		Ink_HashTable *ret = NULL;
		Ink_HashTable *proto_hash = NULL;
		string proto_name = string("$") + getTypeName(type);

		switch (type) {
			case INK_OBJECT:
			case INK_FUNCTION:
			case INK_NUMERIC:
			case INK_BIGNUMERIC:
			case INK_STRING:
			case INK_ARRAY:
				break;
			default:
				proto_name = string("$") + getTypeName(INK_OBJECT);
		}

		while (1) {
			proto_hash = global_context->context->getSlotMapping(NULL, proto_name.c_str());
			if (!(proto_hash && (tmp = proto_hash->getValue()))) return NULL;

			ret = tmp->getSlotMapping(NULL, name);

			if (!ret && type != INK_OBJECT) {
				type = INK_OBJECT;
				proto_name = string("$") + getTypeName(INK_OBJECT);
				continue;
			}
			break;
		}

		return ret;
	}

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

}

#endif
