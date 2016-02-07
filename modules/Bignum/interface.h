#ifndef _NATIVE_INTERFACE_H_
#define _NATIVE_INTERFACE_H_

#include "core/interface/engine.h"
#include "core/object.h"
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
};

#endif
