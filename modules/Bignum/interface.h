#ifndef _MOD_INTERFACE_H_
#define _MOD_INTERFACE_H_

#include "core/interface/engine.h"
#include "core/object.h"
#include "core/constant.h"
#include "bignum.h"

#define BIGNUMERIC_TYPE (getBigNumericType(engine))

using namespace ink;

Ink_TypeTag getBigNumericType(Ink_InterpreteEngine *engine);

class Ink_BigNumeric: public Ink_Object {
public:
	Ink_Bignum_NumericValue value;

	Ink_BigNumeric(Ink_InterpreteEngine *engine, std::string value)
	: Ink_Object(engine), value(Ink_Bignum_NumericValue(value))
	{
		type = BIGNUMERIC_TYPE;
		initProto(engine);
	}

	Ink_BigNumeric(Ink_InterpreteEngine *engine, Ink_Bignum_NumericValue value)
	: Ink_Object(engine), value(value)
	{
		type = BIGNUMERIC_TYPE;
		initProto(engine);
	}

	virtual void derivedMethodInit(Ink_InterpreteEngine *engine)
	{
		Ink_BigNumericMethodInit(engine);
	}
	void Ink_BigNumericMethodInit(Ink_InterpreteEngine *engine);

	virtual Ink_Object *clone(Ink_InterpreteEngine *engine);
	virtual Ink_Object *cloneDeep(Ink_InterpreteEngine *engine);
	virtual bool isTrue()
	{
		return value != 0.0;
	}

	virtual Ink_Constant *toConstant(Ink_InterpreteEngine *engine);
};

struct Ink_BigNumericConstant: public Ink_Constant {
	Ink_Bignum_NumericValue value;

	Ink_BigNumericConstant(Ink_Bignum_NumericValue value)
	: value(value)
	{ }

	virtual Ink_Object *toObject(Ink_InterpreteEngine *engine);
};

extern Ink_ModuleID ink_native_bignum_mod_id;

enum InkMod_Bignum_ExceptionCode {
	INK_EXCODE_WARN_BIGNUM_DIVIDED_BY_ZERO = INK_EXCODE_CUSTOM_START,
	INK_EXCODE_WARN_BIGNUM_FAILED_PARSE_BIGNUM
};

inline void
InkWarn_Bignum_Divided_By_Zero(Ink_InterpreteEngine *engine)
{
	InkErro_doPrintWarning(engine, ink_native_bignum_mod_id,
						   INK_EXCODE_WARN_BIGNUM_DIVIDED_BY_ZERO,
						   "Divided by zero");
	return;
}

inline void
InkWarn_Bignum_Failed_Parse_Bignum(Ink_InterpreteEngine *engine, const char *str)
{
	InkErro_doPrintWarning(engine, ink_native_bignum_mod_id,
						   INK_EXCODE_WARN_BIGNUM_FAILED_PARSE_BIGNUM,
						   "Failed to parse big numeric by string '$(str)'", str);
	return;
}

#endif
