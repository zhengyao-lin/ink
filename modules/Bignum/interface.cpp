#include <math.h>
#include "bignum.h"
#include "interface.h"
#include "core/object.h"
#include "core/native/native.h"
#include "core/native/general.h"

using namespace ink;

Ink_ModuleID ink_native_bignum_mod_id;

Ink_TypeTag getBigNumericType(Ink_InterpreteEngine *engine)
{
	return engine->getEngineComAs<Ink_TypeTag>(ink_native_bignum_mod_id)[0];
}

Ink_Object *Ink_BigNumeric::clone(Ink_InterpreteEngine *engine)
{
	Ink_Object *new_obj = new Ink_BigNumeric(engine, value);

	cloneHashTable(this, new_obj);

	return new_obj;
}

Ink_Object *Ink_BigNumeric::cloneDeep(Ink_InterpreteEngine *engine)
{
	engine->addDeepCloneTrace(this);
	Ink_Object *new_obj = new Ink_BigNumeric(engine, value);

	cloneDeepHashTable(engine, this, new_obj);

	return new_obj;
}

Ink_Object *InkNative_BigNumeric_Add(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value + val);
}

Ink_Object *InkNative_BigNumeric_Sub(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value - val);
}

Ink_Object *InkNative_BigNumeric_Mul(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value * val);
}

Ink_Object *InkNative_BigNumeric_Div(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}

	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;
	Ink_Bignum_NumericValue ret = 0;

	if (val == 0) {
		InkWarn_Bignum_Divided_By_Zero(engine);
		return NULL_OBJ;
	}

	if (argc > 1 && argv[1]->type == INK_NUMERIC) {
		ret = as<Ink_BigNumeric>(base)->value.dividedBy(val, getFloat(as<Ink_Numeric>(argv[1])->getValue()));
	} else {
		ret = as<Ink_BigNumeric>(base)->value / val;
	}

	return new Ink_BigNumeric(engine, ret);
}

Ink_Object *InkNative_BigNumeric_Mod(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value % val);
}

Ink_Object *InkNative_BigNumeric_Spaceship(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value - val);
}

Ink_Object *InkNative_BigNumeric_Equal(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(false, argc, argv, 1, INK_NUMERIC)) {
		InkNote_Method_Fallthrough(engine, "!=", BIGNUMERIC_TYPE, INK_OBJECT);
		return InkNative_Object_Equal(engine, context, argc, argv, this_p);
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value == val);
}

Ink_Object *InkNative_BigNumeric_NotEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(false, argc, argv, 1, INK_NUMERIC)) {
		InkNote_Method_Fallthrough(engine, "==", BIGNUMERIC_TYPE, INK_OBJECT);
		return InkNative_Object_NotEqual(engine, context, argc, argv, this_p);
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value != val);
}

Ink_Object *InkNative_BigNumeric_Greater(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value > val);
}

Ink_Object *InkNative_BigNumeric_Less(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value < val);
}

Ink_Object *InkNative_BigNumeric_GreaterOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value >= val);
}

Ink_Object *InkNative_BigNumeric_LessOrEqual(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	if (!checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)
		&& !checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		return NULL_OBJ;
	}
	Ink_Bignum_NumericValue val = argv[0]->type == INK_NUMERIC
								  ? getFloat(as<Ink_Numeric>(argv[0])->getValue())
								  : as<Ink_BigNumeric>(argv[0])->value;

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value <= val);
}

Ink_Object *InkNative_BigNumeric_Add_Unary(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	return new Ink_BigNumeric(engine, as<Ink_BigNumeric>(base)->value);
}

Ink_Object *InkNative_BigNumeric_Sub_Unary(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	return new Ink_BigNumeric(engine, -as<Ink_BigNumeric>(base)->value);
}

Ink_Object *InkNative_BigNumeric_ToString(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *base = context->searchSlot(engine, "base");

	ASSUME_BASE_TYPE(engine, BIGNUMERIC_TYPE);

	return new Ink_String(engine, as<Ink_BigNumeric>(base)->value.toString());
}

void Ink_BigNumeric::Ink_BigNumericMethodInit(Ink_InterpreteEngine *engine)
{
	setSlot_c("+", new Ink_FunctionObject(engine, InkNative_BigNumeric_Add));
	setSlot_c("-", new Ink_FunctionObject(engine, InkNative_BigNumeric_Sub));
	setSlot_c("*", new Ink_FunctionObject(engine, InkNative_BigNumeric_Mul));
	setSlot_c("/", new Ink_FunctionObject(engine, InkNative_BigNumeric_Div));
	setSlot_c("%", new Ink_FunctionObject(engine, InkNative_BigNumeric_Mod));

	// setSlot_c("&", new Ink_FunctionObject(NULL, InkNative_Numeric_And));
	// setSlot_c("|", new Ink_FunctionObject(NULL, InkNative_Numeric_Or));
	// setSlot_c("^", new Ink_FunctionObject(NULL, InkNative_Numeric_Xor));
	// setSlot_c("<<", new Ink_FunctionObject(NULL, InkNative_Numeric_ShiftLeft));
	// setSlot_c(">>", new Ink_FunctionObject(NULL, InkNative_Numeric_ShiftRight));
	// setSlot_c("~", new Ink_FunctionObject(NULL, InkNative_Numeric_Inverse));

	setSlot_c("div", new Ink_FunctionObject(engine, InkNative_BigNumeric_Div));
	setSlot_c("<=>", new Ink_FunctionObject(engine, InkNative_BigNumeric_Spaceship));
	setSlot_c("==", new Ink_FunctionObject(engine, InkNative_BigNumeric_Equal));
	setSlot_c("!=", new Ink_FunctionObject(engine, InkNative_BigNumeric_NotEqual));
	setSlot_c(">", new Ink_FunctionObject(engine, InkNative_BigNumeric_Greater));
	setSlot_c("<", new Ink_FunctionObject(engine, InkNative_BigNumeric_Less));
	setSlot_c(">=", new Ink_FunctionObject(engine, InkNative_BigNumeric_GreaterOrEqual));
	setSlot_c("<=", new Ink_FunctionObject(engine, InkNative_BigNumeric_LessOrEqual));
	setSlot_c("+u", new Ink_FunctionObject(engine, InkNative_BigNumeric_Add_Unary));
	setSlot_c("-u", new Ink_FunctionObject(engine, InkNative_BigNumeric_Sub_Unary));
	setSlot_c("to_str", new Ink_FunctionObject(engine, InkNative_BigNumeric_ToString));

	return;
}

Ink_Constant *Ink_BigNumeric::toConstant(Ink_InterpreteEngine *engine)
{
	return new Ink_BigNumericConstant(value);
}

Ink_Object *Ink_BigNumericConstant::toObject(Ink_InterpreteEngine *engine)
{
	return new Ink_BigNumeric(engine, value);
}

Ink_Object *InkMod_Bignum_Constructor(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	Ink_Object *ret = NULL_OBJ;
	Ink_Bignum_NumericValue ret_val;
	string tmp_str;

	if (checkArgument(false, argc, argv, 1, BIGNUMERIC_TYPE)) {
		ret_val = Ink_Bignum_NumericValue(as<Ink_BigNumeric>(argv[0])->value);
	} else if (checkArgument(false, argc, argv, 1, INK_STRING)) {
		ret_val = Ink_Bignum_NumericValue(tmp_str = as<Ink_String>(argv[0])->getValue());
		if (!ret_val.isValid()) {
			InkWarn_Bignum_Failed_Parse_Bignum(engine, tmp_str.c_str());
			goto END;
		}
	} else if (checkArgument(engine, argc, argv, 1, INK_NUMERIC)) {
		ret_val = Ink_Bignum_NumericValue(as<Ink_Numeric>(argv[0])->getValue());
	} else {
		return NULL_OBJ;
	}

	ret = new Ink_BigNumeric(engine, ret_val);

END:
	context->getLocal()->setSlot_c("this", ret);

	return ret;
}

struct com_cleaner_arg {
	Ink_ModuleID id;
	com_cleaner_arg(Ink_ModuleID id)
	: id(id)
	{ }
};

struct com_struct {
	Ink_TypeTag bignum_type;

	com_struct()
	: bignum_type(-1)
	{ }
};

void InkMod_Bignum_EngineComCleaner(Ink_InterpreteEngine *engine, void *arg)
{
	com_cleaner_arg *tmp = (com_cleaner_arg *)arg;
	delete engine->getEngineComAs<com_struct>(tmp->id);
	delete tmp;
	return;
}

void InkMod_Bignum_bondType(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
{
	Ink_Object *tmp;
	com_struct *com = NULL;
	Ink_Object *obj_proto = engine->getTypePrototype(INK_OBJECT);

	if (!(com = engine->getEngineComAs<com_struct>(ink_native_bignum_mod_id))) {
		com = new com_struct();

		engine->addEngineCom(ink_native_bignum_mod_id, com);
		engine->addDestructor(Ink_EngineDestructor(InkMod_Bignum_EngineComCleaner,
												   new com_cleaner_arg(ink_native_bignum_mod_id)));
	} else if (com->bignum_type != (Ink_TypeTag)-1) /* has registered */ return;

	com->bignum_type = engine->registerType("bignum");
	context->getGlobal()->setSlot_c("$bignum", tmp = new Ink_BigNumeric(engine, "0"));
	engine->setTypePrototype(com->bignum_type, tmp);
	tmp->setProto(obj_proto);
	tmp->derivedMethodInit(engine);

	return;
}

void InkMod_Bignum_bondTo(Ink_InterpreteEngine *engine, Ink_Object *bondee)
{
	bondee->setSlot_c("bignum", new Ink_FunctionObject(engine, InkMod_Bignum_Constructor));
	return;
}

Ink_Object *InkMod_Bignum_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context, Ink_ArgcType argc, Ink_Object **argv, Ink_Object *this_p)
{
	if (!checkArgument(engine, argc, 2)) {
		return NULL_OBJ;
	}

	Ink_Object *apply_to = argv[1];

	InkMod_Bignum_bondType(engine, context);
	InkMod_Bignum_bondTo(engine, apply_to);

	return NULL_OBJ;
}

extern "C" {
	void InkMod_Loader(Ink_InterpreteEngine *engine, Ink_ContextChain *context)
	{
		addPackage(engine, context, "bignums", new Ink_FunctionObject(engine, InkMod_Bignum_Loader));

		return;
	}

	int InkMod_Init(Ink_ModuleID id)
	{
		ink_native_bignum_mod_id = id;
		return 0;
	}
}
