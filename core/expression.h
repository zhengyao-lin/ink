#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <unistd.h>
#include "object.h"
#include "context.h"
#include "error.h"
#include "gc/collect.h"
#include "thread/thread.h"
#define SET_LINE_NUM (line_num_back = inkerr_current_line_number = line_number)
#define RESTORE_LINE_NUM (inkerr_current_line_number = line_num_back)
using namespace std;

class Ink_Expression;

extern bool CGC_if_return;
extern int inkerr_current_line_number;

typedef vector<Ink_Expression *> Ink_ExpressionList;
typedef vector<string *> Ink_ParamList;

template <class T> T *as(Ink_Expression *obj)
{
	return dynamic_cast<T*>(obj);
}

class Ink_Expression {
public:
	int line_number;
	Ink_Expression()
	: line_number(-1)
	{ }
	virtual Ink_Object *eval(Ink_ContextChain *context_chain) { return NULL; }
	virtual ~Ink_Expression() { }
};

class Ink_GoExpression: public Ink_Expression {
public:
	Ink_Expression *exp;

	Ink_GoExpression(Ink_Expression *exp)
	: exp(exp)
	{ }

	static void *shell(void *p)
	{
		registerThread();
		
		EvalArgument *arg = (EvalArgument *)p;
		Ink_ContextChain *context = arg->context;

		IGC_CollectEngine *gc_engine = new IGC_CollectEngine();
		IGC_initGC(gc_engine);
		gc_engine->initContext(context);

		arg->exp->eval(context);

		gc_engine->collectGarbage(true);
		delete gc_engine;
		delete arg;

		return NULL;
	}

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		pthread_t *thd = (pthread_t *)malloc(sizeof(pthread_t));;

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

class Ink_ReturnExpression: public Ink_Expression {
public:
	Ink_Expression *ret_val;

	Ink_ReturnExpression(Ink_Expression *ret_val)
	: ret_val(ret_val)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		int line_num_back;
		SET_LINE_NUM;

		Ink_Object *ret = ret_val ? ret_val->eval(context_chain) : new Ink_NullObject();
		CGC_if_return = true;
		return ret;

		RESTORE_LINE_NUM;
	}

	~Ink_ReturnExpression()
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

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		int line_num_back;
		bool ret_val = false;
		SET_LINE_NUM;

		if (isTrue(lval->eval(context_chain))) {
			if (type == LOGIC_OR) ret_val = true;
			else if (isTrue(rval->eval(context_chain))) ret_val = true;
		} else if (type == LOGIC_OR && isTrue(rval->eval(context_chain)))
			ret_val = true;

		RESTORE_LINE_NUM;

		return new Ink_Numeric(ret_val);
	}

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

	Ink_AssignmentExpression(Ink_Expression *lval, Ink_Expression *rval)
	: lval(lval), rval(rval)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		int line_num_back;
		SET_LINE_NUM;

		Ink_Object *rval_ret;
		Ink_Object *lval_ret;

		rval_ret = rval->eval(context_chain);
		lval_ret = lval->eval(context_chain);

		if (lval_ret->address) {
			return lval_ret->address->value = rval_ret;
		}

		InkErr_Assigning_Unassignable_Expression(context_chain);
		abort();

		RESTORE_LINE_NUM;
	}

	virtual ~Ink_AssignmentExpression()
	{
		delete lval;
		delete rval;
	}
};

class Ink_TableExpression: public Ink_Expression {
public:
	Ink_ExpressionList elem_list;

	Ink_TableExpression(Ink_ExpressionList elem_list)
	: elem_list(elem_list)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		Ink_ArrayValue val = Ink_ArrayValue();
		unsigned int i;

		for (i = 0; i < elem_list.size(); i++) {
			val.push_back(new Ink_HashTable("", elem_list[i]->eval(context_chain)));
		}

		return new Ink_Array(val);
	}

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

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		int line_num_back;
		SET_LINE_NUM;

		Ink_Object *base_obj = base->eval(context_chain);

		RESTORE_LINE_NUM;
		return getSlot(base_obj, slot_id->c_str());
	}

	static Ink_HashTable *searchPrototype(Ink_Object *obj, const char *id)
	{
		Ink_HashTable *hash = obj->getSlotMapping(id);
		Ink_HashTable *proto;

		if (!hash) {
			proto = obj->getSlotMapping("prototype");
			if (proto && proto->value->type != INK_UNDEFINED) {
				hash = searchPrototype(proto->value, id);
			}
		}

		return hash;
	}

	static Ink_Object *getSlot(Ink_Object *obj, const char *id)
	{
		Ink_HashTable *hash = searchPrototype(obj, id);

		if (obj->type == INK_NULL || obj->type == INK_UNDEFINED) {
			// InkWarn_Get_Null_Hash();
		}

		if (!hash) {
			// InkWarn_Hash_not_found(slot_id->c_str());
			hash = obj->setSlot(id, new Ink_Object(true));
		}
		hash->value->address = hash;

		hash->value->setSlot("base", obj);
		//hash->value->setSlot("this", hash->value);

		return hash->value;
	}

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

	Ink_FunctionExpression(Ink_ParamList param, Ink_ExpressionList exp_list, bool is_inline = false)
	: param(param), exp_list(exp_list), is_inline(is_inline)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain);

	virtual ~Ink_FunctionExpression()
	{
		unsigned int i;
		for (i = 0; i < param.size(); i++) {
			delete param[i];
		}
		for (i = 0; i < exp_list.size(); i++) {
			delete exp_list[i];
		}
	}
};

class Ink_CallExpression: public Ink_Expression {
public:
	Ink_Expression *callee;
	Ink_ExpressionList arg_list;

	Ink_CallExpression(Ink_Expression *callee, Ink_ExpressionList arg_list)
	: callee(callee), arg_list(arg_list)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		int line_num_back;
		SET_LINE_NUM;

		unsigned int i;
		Ink_Object **argv = NULL;
		Ink_Object *ret_val;

		if (arg_list.size()) {
			argv = (Ink_Object **)malloc(arg_list.size() * sizeof(Ink_Object *));
			for (i = 0; i < arg_list.size(); i++) {
				argv[i] = arg_list[i]->eval(context_chain);
			}
		}

		ret_val = callee->eval(context_chain)->call(context_chain, arg_list.size(), argv);

		free(argv);

		RESTORE_LINE_NUM;
		return ret_val;
	}

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

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		int line_num_back;
		SET_LINE_NUM;

		Ink_HashTable *hash;
		Ink_ContextChain *local = context_chain->getLocal();
		Ink_ContextChain *global = context_chain->getGlobal();
		Ink_ContextChain *dest_context = local;

		switch (context_type){
			case ID_LOCAL:
				hash = local->context->getSlotMapping(id->c_str());
				break;
			case ID_GLOBAL:
				hash = global->context->getSlotMapping(id->c_str());
				dest_context = global;
				break;
			default:
				hash = context_chain->searchSlotMapping(id->c_str());
				break;
		}

		if (!hash) hash = dest_context->context->setSlot(id->c_str(), if_create_slot ? new Ink_Object(true) : new Ink_Undefined());
		hash->value->address = hash;
		// hash->value->setSlot("this", hash->value);

		RESTORE_LINE_NUM;
		return hash->value;
	}

	virtual ~Ink_IdentifierExpression()
	{
		delete id;
	}
};

class Ink_NullConstant: public Ink_Expression {
public:
	Ink_NullConstant() { }
	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
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
	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
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

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		int line_num_back;
		SET_LINE_NUM;
		RESTORE_LINE_NUM;
		return new Ink_Numeric(value);
	}

	static Ink_Expression *parse(string code);
	virtual ~Ink_NumericConstant() { }
};

class Ink_StringConstant: public Ink_Expression {
public:
	string *value;

	Ink_StringConstant(string *value)
	: value(value)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
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

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		int line_num_back;
		SET_LINE_NUM;
		Ink_ArrayValue val = Ink_ArrayValue();
		unsigned int i;

		for (i = 0; i < elem_list.size(); i++) {
			val.push_back(new Ink_HashTable("", elem_list[i]->eval(context_chain)));
		}

		RESTORE_LINE_NUM;
		return new Ink_Array(val);
	}

	virtual ~Ink_ArrayLiteral()
	{
		unsigned int i;
		for (i = 0; i < elem_list.size(); i++) {
			delete elem_list[i];
		}
	}
};

class Ink_FloatConstant: public Ink_Expression {
public:
	double value;

	Ink_FloatConstant(double value)
	: value(value)
	{ }
};

#endif