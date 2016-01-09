#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include <iostream>
#include <vector>
#include <stdlib.h>
#include "type.h"
#include "object.h"
#include "context.h"
#include "error.h"
#include "gc/collect.h"
#include "thread/thread.h"
#include "coroutine/coroutine.h"
#include "general.h"
#define SET_LINE_NUM (line_num_back = inkerr_current_line_number = (line_number != -1 ? line_number : inkerr_current_line_number))
#define RESTORE_LINE_NUM (inkerr_current_line_number = line_num_back)
#define INTER_SIGNAL_RECEIVED (CGC_interrupt_signal != INTER_NONE)

using namespace std;

class Ink_Expression;

extern int inkerr_current_line_number;
extern InterruptSignal CGC_interrupt_signal;
extern Ink_Object *CGC_interrupt_value;

typedef vector<Ink_Expression *> Ink_ExpressionList;
// typedef triple<string *, bool, bool> Ink_Parameter;
typedef vector<Ink_Argument *> Ink_ArgumentList;
typedef vector<Ink_Parameter> Ink_ParamList;

template <class T> T *as(Ink_Expression *obj)
{
	return dynamic_cast<T*>(obj);
}

class Ink_EvalFlag {
public:
	bool is_left_value;

	Ink_EvalFlag(bool is_left_value = false)
	: is_left_value(is_left_value)
	{ }
};

class Ink_Expression {
public:
	int line_number;
	Ink_Expression()
	: line_number(-1)
	{ }
	virtual Ink_Object *eval(Ink_ContextChain *context_chain) { return eval(context_chain, Ink_EvalFlag()); }
	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags) { return NULL; }
	virtual Ink_Expression *clone() { return NULL; }
	virtual ~Ink_Expression() { }
};

class Ink_ShellExpression: public Ink_Expression {
public:
	Ink_Object *obj;

	Ink_ShellExpression(Ink_Object *obj)
	: obj(obj)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
	{ return obj; }
};

class Ink_CommaExpression: public Ink_Expression {
public:
	Ink_ExpressionList exp_list;

	Ink_CommaExpression()
	: exp_list(Ink_ExpressionList())
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
	{
		int line_num_back;
		SET_LINE_NUM;

		Ink_Object *ret = new Ink_NullObject();
		unsigned int i;

		for (i = 0; i < exp_list.size(); i++) {
			ret = exp_list[i]->eval(context_chain);
			if (INTER_SIGNAL_RECEIVED) {
				RESTORE_LINE_NUM;
				return CGC_interrupt_value;
			}
		}

		RESTORE_LINE_NUM;
		return ret;
	}

	virtual ~Ink_CommaExpression()
	{
		unsigned int i;
		for (i = 0; i < exp_list.size(); i++) {
			delete exp_list[i];
		}
	}
};

/*
class Ink_GoExpression: public Ink_Expression {
public:
	Ink_Expression *exp;

	Ink_GoExpression(Ink_Expression *exp)
	: exp(exp)
	{ }

	static void *shell(void *p)
	{
		//registerThread();
		
		EvalArgument *arg = (EvalArgument *)p;
		Ink_ContextChain *context = arg->context;

		IGC_CollectEngine *gc_engine = new IGC_CollectEngine();
		IGC_initGC(gc_engine);
		// gc_engine->initContext(context);

		arg->exp->eval(context);

		gc_engine->collectGarbage(true);
		delete gc_engine;
		delete arg;

		return NULL;
	}

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
	{
		pthread_t *thd = (pthread_t *)malloc(sizeof(pthread_t));

		pthread_create(thd, NULL, shell, new EvalArgument(exp, context_chain));
		//sleep(1);
		//pthread_join(thd, NULL);
		//pthread_detach(*thd);
		addThread(thd);

		return new Ink_NullObject();
	}

	~Ink_GoExpression()
	{
		if (exp)
			delete exp;
	}
};
*/

/*
extern Ink_Object *CGC_yield_value;
extern ucontext CGC_yield_from;
extern Ink_Object *CGC_send_back_value;
extern ucontext CGC_yield_to;

class Ink_YieldExpression: public Ink_Expression {
public:
	Ink_Expression *ret_val;

	Ink_YieldExpression(Ink_Expression *ret_val)
	: ret_val(ret_val)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
	{
		int line_num_back;
		SET_LINE_NUM;

		CGC_send_back_value = NULL;
		CGC_yield_value = ret_val->eval(context_chain);
		if (INTER_SIGNAL_RECEIVED) {
			RESTORE_LINE_NUM;
			return CGC_interrupt_value;
		}
		swapcontext(&CGC_yield_from, &CGC_yield_to);

		RESTORE_LINE_NUM;
		return CGC_send_back_value;
	}

	~Ink_YieldExpression()
	{
		delete ret_val;
	}
};
*/

class Ink_InterruptExpression: public Ink_Expression {
public:
	InterruptSignal signal;
	Ink_Expression *ret_val;

	Ink_InterruptExpression(InterruptSignal signal, Ink_Expression *ret_val)
	: signal(signal), ret_val(ret_val)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
	{
		int line_num_back;
		SET_LINE_NUM;

		Ink_Object *ret = ret_val ? ret_val->eval(context_chain) : new Ink_NullObject();
		if (INTER_SIGNAL_RECEIVED) {
			RESTORE_LINE_NUM;
			return CGC_interrupt_value;
		}

		RESTORE_LINE_NUM;
		CGC_interrupt_signal = signal;
		CGC_interrupt_value = ret;

		return ret;
	}

	~Ink_InterruptExpression()
	{
		if (ret_val)
			delete ret_val;
	}
};

typedef enum {
	LOGIC_OR,
	LOGIC_AND
} LogicType;

bool isTrue(Ink_Object *cond);

class Ink_LogicExpression: public Ink_Expression {
public:
	Ink_Expression *lval;
	Ink_Expression *rval;
	LogicType type;

	Ink_LogicExpression(Ink_Expression *lval, Ink_Expression *rval, LogicType type)
	: lval(lval), rval(rval), type(type)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags);

	virtual ~Ink_LogicExpression()
	{
		delete lval;
		delete rval;
	}
};

class Ink_AssignmentExpression: public Ink_Expression {
public:
	Ink_Expression *lval;
	Ink_Expression *rval;
	bool is_return_lval;
	bool is_dispose_lval;

	Ink_AssignmentExpression(Ink_Expression *lval, Ink_Expression *rval,
							 bool is_return_lval = false, bool is_dispose_lval = true)
	: lval(lval), rval(rval), is_return_lval(is_return_lval), is_dispose_lval(is_dispose_lval)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags);

	virtual ~Ink_AssignmentExpression()
	{
		if (is_dispose_lval)
			delete lval;
		delete rval;
	}
};

// typedef pair<string *, Ink_Expression *> Ink_HashTableMappingSingle;

class Ink_HashTableMappingSingle {
public:
	int line_number;
	string *name;
	Ink_Expression *key;
	Ink_Expression *value;

	Ink_HashTableMappingSingle(string *name, Ink_Expression *value)
	: name(name), key(NULL), value(value)
	{ }

	Ink_HashTableMappingSingle(Ink_Expression *key, Ink_Expression *value)
	: name(NULL), key(key), value(value)
	{ }

	~Ink_HashTableMappingSingle()
	{
		if (name) delete name;
		if (key) delete key;
		delete value;
	}
};

typedef vector<Ink_HashTableMappingSingle *> Ink_HashTableMapping;

class Ink_HashTableExpression: public Ink_Expression {
public:
	Ink_HashTableMapping mapping;

	Ink_HashTableExpression(Ink_HashTableMapping mapping)
	: mapping(mapping)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags);

	~Ink_HashTableExpression()
	{
		unsigned int i;
		for (i = 0; i < mapping.size(); i++) {
			delete mapping[i];
		}
	}
};

class Ink_TableExpression: public Ink_Expression {
public:
	Ink_ExpressionList elem_list;

	Ink_TableExpression(Ink_ExpressionList elem_list)
	: elem_list(elem_list)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags);

	virtual ~Ink_TableExpression()
	{
		unsigned int i;
		for (i = 0; i < elem_list.size(); i++) {
			delete elem_list[i];
		}
	}
};

class Ink_HashExpression: public Ink_Expression {
public:
	Ink_Expression *base;
	string *slot_id;

	Ink_HashExpression(Ink_Expression *base, string *slot_id)
	: base(base), slot_id(slot_id)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags);

	class ProtoSearchRet {
	public:
		Ink_HashTable *hash;
		Ink_Object *base;

		ProtoSearchRet(Ink_HashTable *hash = NULL, Ink_Object *base = NULL)
		: hash(hash), base(base)
		{ }
	};

	static ProtoSearchRet searchPrototype(Ink_Object *obj, const char *id);

	static Ink_Object *getSlot(Ink_ContextChain *context_chain, Ink_Object *obj, const char *id)
	{
		return getSlot(context_chain, obj, id, Ink_EvalFlag());
	}
	static Ink_Object *getSlot(Ink_ContextChain *context_chain, Ink_Object *obj, const char *id, Ink_EvalFlag flags);

	virtual ~Ink_HashExpression()
	{
		delete base;
		delete slot_id;
	}
};

class Ink_FunctionExpression: public Ink_Expression {
public:
	Ink_ParamList param;
	Ink_ExpressionList exp_list;
	bool is_inline;
	bool is_generator;

	Ink_FunctionExpression(Ink_ParamList param, Ink_ExpressionList exp_list, bool is_inline = false, bool is_generator = false)
	: param(param), exp_list(exp_list), is_inline(is_inline), is_generator(is_generator)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags);

	virtual ~Ink_FunctionExpression()
	{
		unsigned int i;
		for (i = 0; i < param.size(); i++) {
			delete param[i].name;
		}
		for (i = 0; i < exp_list.size(); i++) {
			delete exp_list[i];
		}
	}
};

class Ink_CallExpression: public Ink_Expression {
public:
	Ink_Expression *callee;
	Ink_ArgumentList arg_list;

	Ink_CallExpression(Ink_Expression *callee, Ink_ArgumentList arg_list)
	: callee(callee), arg_list(arg_list)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags);

	virtual ~Ink_CallExpression()
	{
		unsigned int i;
		delete callee;
		for (i = 0; i < arg_list.size(); i++) {
			delete arg_list[i];
		}
	}
};

typedef enum {
	ID_LOCAL,
	ID_GLOBAL,
	ID_COMMON
} IDContextType;

class Ink_IdentifierExpression: public Ink_Expression {
public:
	string *id;
	IDContextType context_type;
	bool if_create_slot;

	Ink_IdentifierExpression(string *id, IDContextType context_type = ID_COMMON, bool if_create_slot = false)
	: id(id), context_type(context_type), if_create_slot(if_create_slot)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags);

	virtual ~Ink_IdentifierExpression()
	{
		delete id;
	}
};

class Ink_NullConstant: public Ink_Expression {
public:
	Ink_NullConstant() { }
	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
	{
		int line_num_back;
		SET_LINE_NUM;
		RESTORE_LINE_NUM;
		return new Ink_NullObject();
	}
};

class Ink_UndefinedConstant: public Ink_Expression {
public:
	Ink_UndefinedConstant() { }
	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
	{
		int line_num_back;
		SET_LINE_NUM;
		RESTORE_LINE_NUM;
		return new Ink_Undefined();
	}
};

class Ink_NumericConstant: public Ink_Expression {
public:
	double value;

	Ink_NumericConstant(double value)
	: value(value)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
	{
		int line_num_back;
		SET_LINE_NUM;
		RESTORE_LINE_NUM;
		return new Ink_Numeric(value);
	}

	static double parseNumeric(string code, bool *is_success = NULL);
	static Ink_Expression *parse(string code);
	virtual ~Ink_NumericConstant() { }
};

class Ink_StringConstant: public Ink_Expression {
public:
	string *value;

	Ink_StringConstant(string *value)
	: value(value)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags)
	{
		int line_num_back;
		SET_LINE_NUM;
		RESTORE_LINE_NUM;
		return new Ink_String(*value);
	}

	virtual ~Ink_StringConstant()
	{
		delete value;
	}
};

class Ink_ArrayLiteral: public Ink_Expression {
public:
	Ink_ExpressionList elem_list;

	Ink_ArrayLiteral(Ink_ExpressionList elem_list)
	: elem_list(elem_list)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain, Ink_EvalFlag flags);

	virtual ~Ink_ArrayLiteral()
	{
		unsigned int i;
		for (i = 0; i < elem_list.size(); i++) {
			delete elem_list[i];
		}
	}
};

#endif