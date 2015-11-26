#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include <iostream>
#include <vector>
#include <stdlib.h>
#include "object.h"
#include "context.h"
#include "error.h"
using namespace std;

class Ink_Expression;

extern bool CGC_if_return;
extern bool CGC_if_debond;

typedef vector<Ink_Expression *> Ink_ExpressionList;
typedef vector<string *> Ink_ParamList;

template <class T> T *as(Ink_Expression *obj)
{
	return dynamic_cast<T*>(obj);
}

class Ink_Expression {
public:
	Ink_Expression() { }
	virtual Ink_Object *eval(Ink_ContextChain *context_chain) { return NULL; }
	virtual ~Ink_Expression() { }
};

class Ink_ReturnExpression: public Ink_Expression {
public:
	Ink_Expression *ret_val;

	Ink_ReturnExpression(Ink_Expression *ret_val)
	: ret_val(ret_val)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		CGC_if_return = true;
		return ret_val ? ret_val->eval(context_chain) : new Ink_NullObject();
	}

	~Ink_ReturnExpression()
	{
		if (ret_val)
			delete ret_val;
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
		Ink_Object *rval_ret = rval->eval(context_chain);
		Ink_Object *lval_ret = lval->eval(context_chain);

		if (lval_ret->address) {
			return lval_ret->address->value = rval_ret;
		}

		InkErr_Assigning_Unassignable_Expression(context_chain);
		abort();
	}

	virtual ~Ink_AssignmentExpression()
	{
		delete lval;
		delete rval;
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
		Ink_Object *base_obj = base->eval(context_chain);
		return getSlot(base_obj, slot_id->c_str());
	}

	static Ink_Object *getSlot(Ink_Object *obj, const char *id)
	{
		Ink_HashTable *hash = obj->getSlotMapping(id);

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

	bool is_new;

	Ink_CallExpression(Ink_Expression *callee, Ink_ExpressionList arg_list, bool is_new = false)
	: callee(callee), arg_list(arg_list), is_new(is_new)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
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

class Ink_IdentifierExpression: public Ink_Expression {
public:
	string *id;

	Ink_IdentifierExpression(string *id)
	: id(id)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		Ink_HashTable *hash = context_chain->searchSlotMapping(id->c_str());

		if (!hash) hash = context_chain->getLocal()->context->setSlot(id->c_str(), new Ink_Object(true));
		hash->value->address = hash;
		//hash->value->setSlot("this", hash->value);

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
		return new Ink_NullObject();
	}
};

class Ink_UndefinedConstant: public Ink_Expression {
public:
	Ink_UndefinedConstant() { }
	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		return new Ink_Undefined();
	}
};

class Ink_IntegerConstant: public Ink_Expression {
public:
	int value;

	Ink_IntegerConstant(int value)
	: value(value)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
		return new Ink_Integer(value);
	}

	static Ink_Expression *parse(string code);
	virtual ~Ink_IntegerConstant() { }
};

class Ink_StringConstant: public Ink_Expression {
public:
	string *value;

	Ink_StringConstant(string *value)
	: value(value)
	{ }

	virtual Ink_Object *eval(Ink_ContextChain *context_chain)
	{
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
		Ink_ArrayValue val = Ink_ArrayValue();
		unsigned int i;

		for (i = 0; i < elem_list.size(); i++) {
			val.push_back(new Ink_HashTable("", elem_list[i]->eval(context_chain)));
		}

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