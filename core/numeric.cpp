#include "numeric.h"

namespace ink {

Ink_NumericValue Ink_NumericValue::toType(Ink_NumericValue::Ink_NumericValue_NumType t) const
{
	if (t == type) return *this;
	switch (type) {
		case Ink_NumericValue::NUM_INT:
			switch (t) {
				case Ink_NumericValue::NUM_INT:
					return Ink_NumericValue(ival);
				case Ink_NumericValue::NUM_FLOAT:
					return Ink_NumericValue((double)ival);
			}
			break;
		case Ink_NumericValue::NUM_FLOAT:
			switch (t) {
				case Ink_NumericValue::NUM_INT:
					return Ink_NumericValue((Ink_SInt64)fval);
				case Ink_NumericValue::NUM_FLOAT:
					return Ink_NumericValue(fval);
			}
			break;
	}
	assert(0);
	return 0;
}

Ink_NumericValue Ink_NumericValue::operator = (const Ink_NumericValue v1)
{
	type = v1.type;
	switch (type) {
		case Ink_NumericValue::NUM_INT:
			ival = v1.ival;
			break;
		case Ink_NumericValue::NUM_FLOAT:
			fval = v1.fval;
			break;
	}
	return *this;
}

Ink_NumericValue operator + (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type > v2.type ? v1.type : v2.type;

	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return Ink_NumericValue(v1.toType(t).ival + v2.toType(t).ival);
		case Ink_NumericValue::NUM_FLOAT:
			return Ink_NumericValue(v1.toType(t).fval + v2.toType(t).fval);
	}

	assert(0);
	return 0;
}

Ink_NumericValue operator - (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type > v2.type ? v1.type : v2.type;
	
	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return Ink_NumericValue(v1.toType(t).ival - v2.toType(t).ival);
		case Ink_NumericValue::NUM_FLOAT:
			return Ink_NumericValue(v1.toType(t).fval - v2.toType(t).fval);
	}

	assert(0);
	return 0;
}

Ink_NumericValue operator * (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type > v2.type ? v1.type : v2.type;
	
	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return Ink_NumericValue(v1.toType(t).ival * v2.toType(t).ival);
		case Ink_NumericValue::NUM_FLOAT:
			return Ink_NumericValue(v1.toType(t).fval * v2.toType(t).fval);
	}

	assert(0);
	return 0;
}

Ink_NumericValue operator / (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	return Ink_NumericValue(v1.toType(Ink_NumericValue::NUM_FLOAT).fval / v2.toType(Ink_NumericValue::NUM_FLOAT).fval);
}

Ink_NumericValue operator % (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type > v2.type ? v1.type : v2.type;
	
	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return Ink_NumericValue(v1.toType(t).ival % v2.toType(t).ival);
		case Ink_NumericValue::NUM_FLOAT:
			return Ink_NumericValue(fmod(v1.toType(t).fval, v2.toType(t).fval));
	}

	assert(0);
	return 0;
}

Ink_NumericValue operator << (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	return Ink_NumericValue(v1.toType(Ink_NumericValue::NUM_INT).ival << v2.toType(Ink_NumericValue::NUM_INT).ival);
}

Ink_NumericValue operator >> (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	return Ink_NumericValue(v1.toType(Ink_NumericValue::NUM_INT).ival >> v2.toType(Ink_NumericValue::NUM_INT).ival);
}

Ink_NumericValue operator & (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	return Ink_NumericValue(v1.toType(Ink_NumericValue::NUM_INT).ival & v2.toType(Ink_NumericValue::NUM_INT).ival);
}

Ink_NumericValue operator | (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	return Ink_NumericValue(v1.toType(Ink_NumericValue::NUM_INT).ival | v2.toType(Ink_NumericValue::NUM_INT).ival);
}

Ink_NumericValue operator ^ (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	return Ink_NumericValue(v1.toType(Ink_NumericValue::NUM_INT).ival ^ v2.toType(Ink_NumericValue::NUM_INT).ival);
}

Ink_NumericValue operator ~ (const Ink_NumericValue v1)
{
	return Ink_NumericValue(~v1.toType(Ink_NumericValue::NUM_INT).ival);
}

bool operator ! (const Ink_NumericValue v1)
{
	switch (v1.type) {
		case Ink_NumericValue::NUM_INT:
			return !v1.ival;
		case Ink_NumericValue::NUM_FLOAT:
			return !v1.fval;
	}

	assert(0);
	return false;
}

Ink_NumericValue operator - (const Ink_NumericValue v1)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type;

	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return Ink_NumericValue(-v1.toType(t).ival);
		case Ink_NumericValue::NUM_FLOAT:
			return Ink_NumericValue(-v1.toType(t).fval);
	}

	assert(0);
	return 0;
}

Ink_NumericValue operator + (Ink_NumericValue v1)
{
	return v1;
}

Ink_NumericValue operator ++ (Ink_NumericValue &v1)	//++i
{
	switch (v1.type) {
		case Ink_NumericValue::NUM_INT:
			v1.ival++;
			break;
		case Ink_NumericValue::NUM_FLOAT:
			v1.fval++;
			break;
	}
	return v1;
}

Ink_NumericValue operator ++ (Ink_NumericValue &v1, int x)	//i++
{
	Ink_NumericValue tmp = v1;
	switch (v1.type) {
		case Ink_NumericValue::NUM_INT:
			v1.ival++;
			break;
		case Ink_NumericValue::NUM_FLOAT:
			v1.fval++;
			break;
	}
	return tmp;
}

Ink_NumericValue operator -- (Ink_NumericValue &v1)	//--i
{
	switch (v1.type) {
		case Ink_NumericValue::NUM_INT:
			v1.ival--;
			break;
		case Ink_NumericValue::NUM_FLOAT:
			v1.fval--;
			break;
	}
	return v1;
}

Ink_NumericValue operator -- (Ink_NumericValue &v1, int x)	//i--
{
	Ink_NumericValue tmp = v1;
	switch (v1.type) {
		case Ink_NumericValue::NUM_INT:
			v1.ival--;
			break;
		case Ink_NumericValue::NUM_FLOAT:
			v1.fval--;
			break;
	}
	return tmp;
}

bool operator > (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type > v2.type ? v1.type : v2.type;
	
	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return v1.toType(t).ival > v2.toType(t).ival;
		case Ink_NumericValue::NUM_FLOAT:
			return v1.toType(t).fval > v2.toType(t).fval;
	}

	assert(0);
	return false;
}

bool operator < (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type > v2.type ? v1.type : v2.type;
	
	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return v1.toType(t).ival < v2.toType(t).ival;
		case Ink_NumericValue::NUM_FLOAT:
			return v1.toType(t).fval < v2.toType(t).fval;
	}

	assert(0);
	return false;
}

bool operator == (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type > v2.type ? v1.type : v2.type;
	
	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return v1.toType(t).ival == v2.toType(t).ival;
		case Ink_NumericValue::NUM_FLOAT:
			return v1.toType(t).fval == v2.toType(t).fval;
	}

	assert(0);
	return false;
}

bool operator != (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type > v2.type ? v1.type : v2.type;
	
	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return v1.toType(t).ival != v2.toType(t).ival;
		case Ink_NumericValue::NUM_FLOAT:
			return v1.toType(t).fval != v2.toType(t).fval;
	}

	assert(0);
	return false;
}

bool operator >= (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type > v2.type ? v1.type : v2.type;
	
	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return v1.toType(t).ival >= v2.toType(t).ival;
		case Ink_NumericValue::NUM_FLOAT:
			return v1.toType(t).fval >= v2.toType(t).fval;
	}

	assert(0);
	return false;
}

bool operator <= (const Ink_NumericValue v1, const Ink_NumericValue v2)
{
	Ink_NumericValue::Ink_NumericValue_NumType t = v1.type > v2.type ? v1.type : v2.type;
	
	switch (t) {
		case Ink_NumericValue::NUM_INT:
			return v1.toType(t).ival <= v2.toType(t).ival;
		case Ink_NumericValue::NUM_FLOAT:
			return v1.toType(t).fval <= v2.toType(t).fval;
	}

	assert(0);
	return false;
}

ostream &operator << (ostream &strm, const Ink_NumericValue v1)
{
	return strm << v1.toString();
}

}
