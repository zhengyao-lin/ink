#include <math.h>
#include "../object.h"
#include "../context.h"
#include "../interface/engine.h"
#include "../gc/collect.h"
#include "native.h"

namespace ink {

using namespace std;

class Ink_Numeric;

Ink_Object *InkNative_Numeric_Add(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() + as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Sub(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() - as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Mul(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() * as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Div(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() / as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Mod(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() % as<Ink_Numeric>(argv[0])->getValue());
}

/*
inline Ink_SInt64
getIntegerVal(Ink_Object *num)
{
	return (Ink_SInt64)(as<Ink_Numeric>(num)->getValue());
}
*/

Ink_Object *InkNative_Numeric_And(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() & as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Or(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() | as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Xor(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() ^ as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_ShiftLeft(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() << as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_ShiftRight(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() >> as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Inverse(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, ~(as<Ink_Numeric>(base)->getValue()));
}

Ink_Object *InkNative_Numeric_Spaceship(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() - as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Equal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(false, argc, argv, 1, INK_NUMERIC)) {
		InkNote_Method_Fallthrough(engine, "==", INK_NUMERIC, INK_OBJECT);
		return InkNative_Object_Equal(engine, context, argc, argv, this_p);
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() == as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_NotEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	if (!argc) return NULL_OBJ;

	if (!checkArgument(false, argc, argv, 1, INK_NUMERIC)) {
		InkNote_Method_Fallthrough(engine, "!=", INK_NUMERIC, INK_OBJECT);
		return InkNative_Object_NotEqual(engine, context, argc, argv, this_p);
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() != as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Greater(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() > as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Less(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() < as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_GreaterOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() >= as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_LessOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (!checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue() <= as<Ink_Numeric>(argv[0])->getValue());
}

Ink_Object *InkNative_Numeric_Add_Unary(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, as<Ink_Numeric>(base)->getValue());
}

Ink_Object *InkNative_Numeric_Sub_Unary(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, -as<Ink_Numeric>(base)->getValue());
}

Ink_Object *InkNative_Numeric_Not_Postfix(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	Ink_NumericValue val = as<Ink_Numeric>(base)->getValue();
	Ink_NumericValue ret, i;
	for (i = 1, ret = 1; i <= val; i++) ret = ret * i;

	return new Ink_Numeric(engine, ret);
}

Ink_Object *InkNative_Numeric_ToString(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	stringstream ss;

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);
	ss << as<Ink_Numeric>(base)->getValue();

	return new Ink_String(engine, string(ss.str()));
}

Ink_Object *InkNative_Numeric_Ceil(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, ceil(getFloat(as<Ink_Numeric>(base)->getValue())));
}

Ink_Object *InkNative_Numeric_Floor(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, floor(getFloat(as<Ink_Numeric>(base)->getValue())));
}

Ink_Object *InkNative_Numeric_Round(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, round(getFloat(as<Ink_Numeric>(base)->getValue())));
}

Ink_Object *InkNative_Numeric_Trunc(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, getInt(as<Ink_Numeric>(base)->getValue()));
}

Ink_Object *InkNative_Numeric_Abs(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, fabs(getFloat(as<Ink_Numeric>(base)->getValue())));
}

Ink_Object *InkNative_Numeric_IsNan(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, isnan((long double)getFloat(as<Ink_Numeric>(base)->getValue())));
}

Ink_Object *InkNative_Numeric_IsInf(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	return new Ink_Numeric(engine, isinf((long double)getFloat(as<Ink_Numeric>(base)->getValue())));
}

Ink_Object *InkNative_Numeric_Times(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");
	Ink_NumericValue i, to;
	Ink_Object *block = NULL;
	Ink_Object **args;
	Ink_Array *ret = NULL;
	IGC_CollectEngine *gc_engine = engine->getCurrentGC();

	ASSUME_BASE_TYPE(engine, INK_NUMERIC);

	if (argc > 0 && argv[0]->type == INK_FUNCTION) {
		block = argv[0];
	}

	to = as<Ink_Numeric>(base)->getValue();
	ret = new Ink_Array(engine);
	engine->addPardonObject(ret);

	args = (Ink_Object **)malloc(sizeof(Ink_Object *));
	for (i = 0; i < to; i++) {
		gc_engine->checkGC();
		if (block) {
			args[0] = new Ink_Numeric(engine, i);
			ret->value.push_back(new Ink_HashTable(argv[0]->call(engine, context, 1, args)));
			if (engine->getSignal() != INTER_NONE) {
				switch (engine->getSignal()) {
					case INTER_RETURN:
						free(args);
						engine->removePardonObject(ret);
						return engine->getInterruptValue(); // signal penetrated
					case INTER_DROP:
					case INTER_BREAK:
						free(args);
						engine->removePardonObject(ret);
						return engine->trapSignal(); // trap the signal
					case INTER_CONTINUE:
						engine->trapSignal(); // trap the signal, but do not return
						continue;
					default:
						free(args);
						engine->removePardonObject(ret);
						return NULL_OBJ;
				}
			}
		} else {
			ret->value.push_back(new Ink_HashTable(new Ink_Numeric(engine, i)));
		}
	}

	free(args);
	engine->removePardonObject(ret);

	return ret;
}

void Ink_Numeric::Ink_NumericMethodInit(Ink_InterpreteEngine *engine)
{
	setSlot_c("+", new Ink_FunctionObject(engine, InkNative_Numeric_Add));
	setSlot_c("-", new Ink_FunctionObject(engine, InkNative_Numeric_Sub));
	setSlot_c("*", new Ink_FunctionObject(engine, InkNative_Numeric_Mul));
	setSlot_c("/", new Ink_FunctionObject(engine, InkNative_Numeric_Div));
	setSlot_c("%", new Ink_FunctionObject(engine, InkNative_Numeric_Mod));
	setSlot_c("&", new Ink_FunctionObject(engine, InkNative_Numeric_And));
	setSlot_c("|", new Ink_FunctionObject(engine, InkNative_Numeric_Or));
	setSlot_c("^", new Ink_FunctionObject(engine, InkNative_Numeric_Xor));
	setSlot_c("<<", new Ink_FunctionObject(engine, InkNative_Numeric_ShiftLeft));
	setSlot_c(">>", new Ink_FunctionObject(engine, InkNative_Numeric_ShiftRight));
	setSlot_c("~", new Ink_FunctionObject(engine, InkNative_Numeric_Inverse));
	setSlot_c("<=>", new Ink_FunctionObject(engine, InkNative_Numeric_Spaceship));
	setSlot_c("==", new Ink_FunctionObject(engine, InkNative_Numeric_Equal));
	setSlot_c("!=", new Ink_FunctionObject(engine, InkNative_Numeric_NotEqual));
	setSlot_c(">", new Ink_FunctionObject(engine, InkNative_Numeric_Greater));
	setSlot_c("<", new Ink_FunctionObject(engine, InkNative_Numeric_Less));
	setSlot_c(">=", new Ink_FunctionObject(engine, InkNative_Numeric_GreaterOrEqual));
	setSlot_c("<=", new Ink_FunctionObject(engine, InkNative_Numeric_LessOrEqual));
	setSlot_c("+u", new Ink_FunctionObject(engine, InkNative_Numeric_Add_Unary));
	setSlot_c("-u", new Ink_FunctionObject(engine, InkNative_Numeric_Sub_Unary));
	setSlot_c("!p", new Ink_FunctionObject(engine, InkNative_Numeric_Not_Postfix));
	setSlot_c("to_str", new Ink_FunctionObject(engine, InkNative_Numeric_ToString));
	setSlot_c("ceil", new Ink_FunctionObject(engine, InkNative_Numeric_Ceil));
	setSlot_c("floor", new Ink_FunctionObject(engine, InkNative_Numeric_Floor));
	setSlot_c("round", new Ink_FunctionObject(engine, InkNative_Numeric_Round));
	setSlot_c("trunc", new Ink_FunctionObject(engine, InkNative_Numeric_Trunc));
	setSlot_c("abs", new Ink_FunctionObject(engine, InkNative_Numeric_Abs));
	setSlot_c("isnan", new Ink_FunctionObject(engine, InkNative_Numeric_IsNan));
	setSlot_c("isinf", new Ink_FunctionObject(engine, InkNative_Numeric_IsInf));

	setSlot_c("times", new Ink_FunctionObject(engine, InkNative_Numeric_Times));
	
	return;
}

}
