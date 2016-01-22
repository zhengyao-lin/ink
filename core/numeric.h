#ifndef _NUMERIC_H_
#define _NUMERIC_H_

#include <deque>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <math.h>
#include <float.h>

using namespace std;

class Ink_BigNumericValue;

class Ink_BigInteger {
	vector<char> digits;
	bool sign;
	void trim();

public:
	friend class Ink_BigNumericValue;
	Ink_BigInteger(long);
	Ink_BigInteger(double, bool);
	Ink_BigInteger(string);
	Ink_BigInteger();
	Ink_BigInteger(const Ink_BigInteger &);
	Ink_BigInteger operator = (const Ink_BigInteger &op2);

	Ink_BigInteger abs() const;
	Ink_BigInteger pow(int a);
	Ink_BigInteger pow(Ink_BigInteger a);

	friend Ink_BigInteger operator += (Ink_BigInteger &, const Ink_BigInteger &);
	friend Ink_BigInteger operator -= (Ink_BigInteger &, const Ink_BigInteger &);
	friend Ink_BigInteger operator *= (Ink_BigInteger &, const Ink_BigInteger &);
	friend Ink_BigInteger operator /= (Ink_BigInteger &, const Ink_BigInteger &);
	friend Ink_BigInteger operator %= (Ink_BigInteger &, const Ink_BigInteger &);

	friend Ink_BigInteger operator + (const Ink_BigInteger &, const Ink_BigInteger &);
	friend Ink_BigInteger operator - (const Ink_BigInteger &, const Ink_BigInteger &);
	friend Ink_BigInteger operator * (const Ink_BigInteger &, const Ink_BigInteger &);
	friend Ink_BigInteger operator / (const Ink_BigInteger &, const Ink_BigInteger &);
	friend Ink_BigInteger operator % (const Ink_BigInteger &, const Ink_BigInteger &);


	friend Ink_BigInteger operator - (const Ink_BigInteger &);

	friend Ink_BigInteger operator ++ (Ink_BigInteger &);		//++i
	friend Ink_BigInteger operator ++ (Ink_BigInteger &, int);	//i++
	friend Ink_BigInteger operator -- (Ink_BigInteger &);		//--i
	friend Ink_BigInteger operator -- (Ink_BigInteger &, int);	//i--

	friend bool operator > (const Ink_BigInteger &, const Ink_BigInteger &);
	friend bool operator < (const Ink_BigInteger &, const Ink_BigInteger &);
	friend bool operator == (const Ink_BigInteger &, const Ink_BigInteger &);
	friend bool operator != (const Ink_BigInteger &, const Ink_BigInteger &);
	friend bool operator >= (const Ink_BigInteger &, const Ink_BigInteger &);
	friend bool operator <= (const Ink_BigInteger &, const Ink_BigInteger &);

	// friend ostream &operator << (ostream &, const Ink_BigInteger &);
	// friend istream &operator >> (istream &, Ink_BigInteger &);

	static const Ink_BigInteger Zero;
	static const Ink_BigInteger One;
	static const Ink_BigInteger Ten;

	friend Ink_BigNumericValue operator += (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs);
	friend Ink_BigNumericValue operator -= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs);
	friend Ink_BigNumericValue operator *= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs);
	friend Ink_BigNumericValue operator /= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs);
	friend Ink_BigNumericValue operator %= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs);
};

class Ink_BigNumericValue {
	Ink_BigInteger num;
	int std_pow;
public:

	enum {
		DEFAULT_ACC = 16
	};

	static const Ink_BigNumericValue Zero;
	static const Ink_BigNumericValue One;
	static const Ink_BigNumericValue Ten;

	Ink_BigNumericValue()
	{
		Ink_BigNumericValue(0.0);
	}

	Ink_BigNumericValue(double val);
    Ink_BigNumericValue(string str);
    Ink_BigNumericValue(Ink_BigInteger int_val);

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
		for (vector<char>::const_reverse_iterator iter = num.digits.rbegin();
			 iter != num.digits.rend() ; iter++, pos++) {
			if (pos == std_pow) {
				ret += ".";
			}
			buffer[0] = (char)((*iter) + '0');
			ret += string(buffer);
		}

		return ret;
	}

	inline long getDecimal()
	{
		return num.digits.size() - std_pow;
	}

	inline long getDecimal() const
	{
		return num.digits.size() - std_pow;
	}

	inline Ink_BigNumericValue dividedBy(const Ink_BigNumericValue &rhs, long prec = DEFAULT_ACC)
	{
		prec += rhs.getDecimal();
		Ink_BigInteger acc = Ink_BigInteger(10).pow(prec);
		Ink_BigInteger op1 = (num * acc);

		Ink_BigNumericValue ret = Ink_BigNumericValue(op1 / rhs.num);
		ret.std_pow = ret.num.digits.size() - prec - getDecimal() + rhs.getDecimal();

		return ret;
	}

	friend Ink_BigNumericValue operator += (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs);
	friend Ink_BigNumericValue operator -= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs);
	friend Ink_BigNumericValue operator *= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs);
	friend Ink_BigNumericValue operator /= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs);
	friend Ink_BigNumericValue operator %= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs);

	friend Ink_BigNumericValue operator + (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);
	friend Ink_BigNumericValue operator - (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);
	friend Ink_BigNumericValue operator * (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);
	friend Ink_BigNumericValue operator / (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);
	friend Ink_BigNumericValue operator % (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);

	friend Ink_BigNumericValue operator - (const Ink_BigNumericValue &op);
	friend Ink_BigNumericValue operator + (const Ink_BigNumericValue &op);

	friend Ink_BigNumericValue operator ++ (Ink_BigNumericValue &op); // ++v
	friend Ink_BigNumericValue operator -- (Ink_BigNumericValue &op); // --v
	friend Ink_BigNumericValue operator ++ (Ink_BigNumericValue &op, int a); // v++
	friend Ink_BigNumericValue operator -- (Ink_BigNumericValue &op, int a); // v--

	friend bool operator < (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);
	friend bool operator == (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);
	friend bool operator != (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);
	friend bool operator > (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);
	friend bool operator <= (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);
	friend bool operator >= (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2);
};

#endif