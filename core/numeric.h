#ifndef _NUMERIC_H_
#define _NUMERIC_H_

#include <sstream>
#include <math.h>
#include <assert.h>
#include "inttype.h"

namespace ink {

using namespace std;

class Ink_NumericValue {
public:
	enum Ink_NumericValue_NumType {
		NUM_INT = 0,
		NUM_FLOAT = 1
	} type;

	union {
		Ink_SInt64 ival;
		double fval;
	};

	Ink_NumericValue()
	{
		type = NUM_INT;
		ival = 0;
	}

	Ink_NumericValue(int val)
	{
		type = NUM_INT;
		ival = val;
	}

	explicit Ink_NumericValue(Ink_SInt64 val)
	{
		type = NUM_INT;
		ival = val;
	}

	explicit Ink_NumericValue(double val)
	{
		type = NUM_FLOAT;
		fval = val;
	}

	inline string toString() const
	{
		stringstream strm;
		switch (type) {
			case NUM_INT:
				strm << ival;
				break;
			case NUM_FLOAT:
				strm << fval;
				break;
		}
		return string(strm.str());
	}

	inline Ink_SInt64 toInt()
	{
		return toType(NUM_INT).ival;
	}

	inline double toFloat()
	{
		return toType(NUM_FLOAT).fval;
	}

	inline bool toBool()
	{
		switch (type) {
			case NUM_INT:
				return ival;
			case NUM_FLOAT:
				return fval;
		}
		assert(0);
	}

	Ink_NumericValue toType(Ink_NumericValue_NumType t) const;

	Ink_NumericValue operator = (const Ink_NumericValue v1);

	friend Ink_NumericValue operator + (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend Ink_NumericValue operator - (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend Ink_NumericValue operator * (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend Ink_NumericValue operator / (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend Ink_NumericValue operator % (const Ink_NumericValue v1, const Ink_NumericValue v2);

	friend Ink_NumericValue operator << (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend Ink_NumericValue operator >> (const Ink_NumericValue v1, const Ink_NumericValue v2);

	friend Ink_NumericValue operator & (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend Ink_NumericValue operator | (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend Ink_NumericValue operator ^ (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend Ink_NumericValue operator ~ (const Ink_NumericValue v1);
	friend bool operator ! (const Ink_NumericValue v1);

	friend Ink_NumericValue operator - (const Ink_NumericValue v1);
	friend Ink_NumericValue operator + (Ink_NumericValue v1);

	friend Ink_NumericValue operator ++ (Ink_NumericValue &v1);			//++i
	friend Ink_NumericValue operator ++ (Ink_NumericValue &v1, int x);	//i++
	friend Ink_NumericValue operator -- (Ink_NumericValue &v1);			//--i
	friend Ink_NumericValue operator -- (Ink_NumericValue &v1, int x);	//i--

	friend bool operator > (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend bool operator < (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend bool operator == (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend bool operator != (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend bool operator >= (const Ink_NumericValue v1, const Ink_NumericValue v2);
	friend bool operator <= (const Ink_NumericValue v1, const Ink_NumericValue v2);

	friend ostream &operator << (ostream &strm, const Ink_NumericValue v1);
};

}

#endif
