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
};

class Ink_BigNumericValue {
	Ink_BigInteger num;
	int std_pow;
public:

	enum {
		DEFAULT_ACC = 16
	};

	Ink_BigNumericValue()
	{
		Ink_BigNumericValue(0.0);
	}

	Ink_BigNumericValue(double val)
    {
		num = std_pow = 0;
		if (!isnan(val)) {
			double tmp_val = val;
			Ink_BigInteger integer(floor(tmp_val), true);
			Ink_BigInteger decimal((tmp_val - floor(tmp_val)) * pow(10, DEFAULT_ACC), true);
			num = integer * Ink_BigInteger(10).pow(DEFAULT_ACC) + decimal;
			//std::cout << integer << decimal << endl;
			std_pow = fabs(tmp_val) >= 1
					  ? integer.digits.size()
					  : decimal.digits.size() - DEFAULT_ACC;
		}
    }

    Ink_BigNumericValue(string str)
    {
		unsigned int pos;
		num = std_pow = 0;
		int sign = 1;
		Ink_BigInteger integer = 0;
		string decimal;

		if (str.length()) {
			if (str[0] == '-') {
        		str = str.substr(1);
        		sign = -1;
        	} else if (str[0] == '+') str = str.substr(1);

			pos = str.find_first_of('.');
			if (pos != (unsigned int)-1) {
				integer = Ink_BigInteger(str.substr(0, pos));
				decimal = str.substr(pos + 1);
        		num = sign * (integer.abs() * Ink_BigInteger(10).pow(decimal.length()) + Ink_BigInteger(decimal).abs());
				std_pow = integer > 0 ? integer.digits.size() : num.digits.size() - decimal.length();
			} else {
				num = sign * Ink_BigInteger(str);
				std_pow = num.digits.size();
			}
		}
    }

    Ink_BigNumericValue(Ink_BigInteger int_val)
    {
        num = int_val;
        std_pow = int_val.digits.size();
    }

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

	friend Ink_BigNumericValue operator += (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs)
	{
		Ink_BigInteger tmp_lhs;
		Ink_BigInteger tmp_rhs;
		Ink_BigInteger ten = Ink_BigInteger::Ten;
		long sup = lhs.getDecimal() - rhs.getDecimal();

		if (sup > 0) {
			tmp_lhs = lhs.num;
			tmp_rhs = rhs.num * ten.pow(abs(sup));
		} else {
			tmp_lhs = lhs.num * ten.pow(abs(sup));
			tmp_rhs = rhs.num;
		}

		Ink_BigNumericValue ret = Ink_BigNumericValue(tmp_lhs + tmp_rhs);
		ret.std_pow = max(lhs.std_pow, rhs.std_pow) + (ret.num.digits.size() - max(tmp_lhs.digits.size(), tmp_rhs.digits.size()));
		lhs = ret;

		return ret;
	}

	friend Ink_BigNumericValue operator -= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs)
	{
		Ink_BigInteger tmp_lhs;
		Ink_BigInteger tmp_rhs;
		Ink_BigInteger ten = Ink_BigInteger::Ten;
		long sup = lhs.getDecimal() - rhs.getDecimal();

		if (sup > 0) {
			tmp_lhs = lhs.num;
			tmp_rhs = rhs.num * ten.pow(abs(sup));
		} else {
			tmp_lhs = lhs.num * ten.pow(abs(sup));
			tmp_rhs = rhs.num;
		}

		Ink_BigNumericValue ret = Ink_BigNumericValue(tmp_lhs - tmp_rhs);
		ret.std_pow = max(lhs.std_pow, rhs.std_pow) + (ret.num.digits.size() - max(tmp_lhs.digits.size(), tmp_rhs.digits.size()));
		lhs = ret;

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

	friend Ink_BigNumericValue operator *= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs)
	{
		Ink_BigNumericValue ret = Ink_BigNumericValue(lhs.num * rhs.num);
		ret.std_pow = ret.num.digits.size() - (lhs.getDecimal() + rhs.getDecimal());
		lhs = ret;

		return ret;
	}

	friend Ink_BigNumericValue operator /= (Ink_BigNumericValue &lhs, const Ink_BigNumericValue &rhs)
	{
		return lhs = lhs.dividedBy(rhs);
	}

	friend Ink_BigNumericValue operator + (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2)
	{
		Ink_BigNumericValue temp(op1);
		temp += op2;
		return temp;
	}

	friend Ink_BigNumericValue operator - (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2)
	{
		Ink_BigNumericValue temp(op1);
		temp -= op2;
		return temp;
	}

	friend Ink_BigNumericValue operator * (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2)
	{
		Ink_BigNumericValue temp(op1);
		temp *= op2;
		return temp;
	}

	friend Ink_BigNumericValue operator / (const Ink_BigNumericValue &op1, const Ink_BigNumericValue &op2)
	{
		Ink_BigNumericValue temp(op1);
		temp /= op2;
		return temp;
	}

	friend Ink_BigNumericValue operator - (const Ink_BigNumericValue &op)
	{
		Ink_BigNumericValue temp(op);
		temp.num = -temp.num;
		return temp;
	}

	friend Ink_BigNumericValue operator + (const Ink_BigNumericValue &op)
	{
		return op;
	}
};

#endif