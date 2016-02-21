#ifndef _NATIVE_BIGNUM_H_
#define _NATIVE_BIGNUM_H_

#include <deque>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <math.h>
#include <float.h>

using namespace std;

class Ink_Bignum_NumericValue;

typedef vector<char> Ink_Bignum_Integer_Digit;

class Ink_Bignum_Integer {
	Ink_Bignum_Integer_Digit digits;
	bool sign;
	void trim();

public:
	friend class Ink_Bignum_NumericValue;
	Ink_Bignum_Integer(double);
	Ink_Bignum_Integer(string);

	Ink_Bignum_Integer(Ink_Bignum_Integer_Digit digits, bool sign)
	: digits(digits), sign(sign)
	{ }
	
	Ink_Bignum_Integer();
	Ink_Bignum_Integer(const Ink_Bignum_Integer &);
	Ink_Bignum_Integer operator = (const Ink_Bignum_Integer &op2);

	Ink_Bignum_Integer abs() const;
	Ink_Bignum_Integer pow(int a);
	Ink_Bignum_Integer pow(Ink_Bignum_Integer a);
	Ink_Bignum_Integer exp(long a);

	inline long toLong()
	{
		Ink_Bignum_Integer_Digit::reverse_iterator iter;
		long ret = 0;

		for (iter = digits.rbegin();
			 iter != digits.rend(); iter++) {
			ret *= 10;
			ret += *iter;
		}

		return ret;
	}

	inline bool isValid()
	{
		return digits.size() > 0;
	}

	friend Ink_Bignum_Integer operator += (Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend Ink_Bignum_Integer operator -= (Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend Ink_Bignum_Integer operator *= (Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend Ink_Bignum_Integer operator /= (Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend Ink_Bignum_Integer operator %= (Ink_Bignum_Integer &, const Ink_Bignum_Integer &);

	friend Ink_Bignum_Integer operator + (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend Ink_Bignum_Integer operator - (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend Ink_Bignum_Integer operator * (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend Ink_Bignum_Integer operator / (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend Ink_Bignum_Integer operator % (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);


	friend Ink_Bignum_Integer operator - (const Ink_Bignum_Integer &);

	friend Ink_Bignum_Integer operator ++ (Ink_Bignum_Integer &);		//++i
	friend Ink_Bignum_Integer operator ++ (Ink_Bignum_Integer &, int);	//i++
	friend Ink_Bignum_Integer operator -- (Ink_Bignum_Integer &);		//--i
	friend Ink_Bignum_Integer operator -- (Ink_Bignum_Integer &, int);	//i--

	friend bool operator > (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend bool operator < (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend bool operator == (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend bool operator != (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend bool operator >= (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);
	friend bool operator <= (const Ink_Bignum_Integer &, const Ink_Bignum_Integer &);

	// friend ostream &operator << (ostream &, const Ink_Bignum_Integer &);
	// friend istream &operator >> (istream &, Ink_Bignum_Integer &);

	static const Ink_Bignum_Integer Zero;
	static const Ink_Bignum_Integer One;
	static const Ink_Bignum_Integer Ten;

	friend Ink_Bignum_NumericValue operator += (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs);
	friend Ink_Bignum_NumericValue operator -= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs);
	friend Ink_Bignum_NumericValue operator *= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs);
	friend Ink_Bignum_NumericValue operator /= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs);
	friend Ink_Bignum_NumericValue operator %= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs);
};

class Ink_Bignum_NumericValue {
	Ink_Bignum_Integer num;
	long std_pow;
public:

	enum {
		DEFAULT_ACC = 16
	};

	static const Ink_Bignum_NumericValue Zero;
	static const Ink_Bignum_NumericValue One;
	static const Ink_Bignum_NumericValue Ten;

	Ink_Bignum_NumericValue()
	{
		Ink_Bignum_NumericValue(0.0);
	}

	Ink_Bignum_NumericValue(double val);
    Ink_Bignum_NumericValue(string str);
    Ink_Bignum_NumericValue(Ink_Bignum_Integer int_val);

	inline string toString()
	{
		string ret;
		char buffer[2];
		buffer[1] = '\0';

		if (!num.sign)
			ret += "-";

		if (std_pow < 0) {
        	long i = -std_pow;
        	ret += ".";
        	while (i > 0) {
        		ret += "0";
        		i--;
        	}
        }

		long pos = 0;
		for (Ink_Bignum_Integer_Digit::const_reverse_iterator iter = num.digits.rbegin();
			 iter != num.digits.rend() ; iter++, pos++) {
			if (pos == std_pow) {
				ret += ".";
			}
			buffer[0] = (char)((*iter) + '0');
			ret += string(buffer);
		}

		return ret;
	}

	inline bool isValid()
	{
		return num.isValid();
	}

	inline long getDecimal()
	{
		return num.digits.size() - std_pow;
	}

	inline long getDecimal() const
	{
		return num.digits.size() - std_pow;
	}

	inline Ink_Bignum_NumericValue dividedBy(const Ink_Bignum_NumericValue &rhs, long prec = DEFAULT_ACC)
	{
		prec += rhs.getDecimal();
		Ink_Bignum_Integer acc = Ink_Bignum_Integer(10).pow(prec);
		Ink_Bignum_Integer op1 = (num * acc);

		Ink_Bignum_NumericValue ret = Ink_Bignum_NumericValue(op1 / rhs.num);
		ret.std_pow = ret.num.digits.size() - prec - getDecimal() + rhs.getDecimal();

		return ret;
	}

	friend Ink_Bignum_NumericValue operator += (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs);
	friend Ink_Bignum_NumericValue operator -= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs);
	friend Ink_Bignum_NumericValue operator *= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs);
	friend Ink_Bignum_NumericValue operator /= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs);
	friend Ink_Bignum_NumericValue operator %= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs);

	friend Ink_Bignum_NumericValue operator + (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);
	friend Ink_Bignum_NumericValue operator - (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);
	friend Ink_Bignum_NumericValue operator * (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);
	friend Ink_Bignum_NumericValue operator / (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);
	friend Ink_Bignum_NumericValue operator % (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);

	friend Ink_Bignum_NumericValue operator - (const Ink_Bignum_NumericValue &op);
	friend Ink_Bignum_NumericValue operator + (const Ink_Bignum_NumericValue &op);

	friend Ink_Bignum_NumericValue operator ++ (Ink_Bignum_NumericValue &op); // ++v
	friend Ink_Bignum_NumericValue operator -- (Ink_Bignum_NumericValue &op); // --v
	friend Ink_Bignum_NumericValue operator ++ (Ink_Bignum_NumericValue &op, int a); // v++
	friend Ink_Bignum_NumericValue operator -- (Ink_Bignum_NumericValue &op, int a); // v--

	friend bool operator < (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);
	friend bool operator == (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);
	friend bool operator != (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);
	friend bool operator > (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);
	friend bool operator <= (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);
	friend bool operator >= (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2);
};

#endif
