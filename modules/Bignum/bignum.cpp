#include <stdio.h>
#include "bignum.h"

using namespace std;

const Ink_Bignum_Integer Ink_Bignum_Integer::Zero = Ink_Bignum_Integer(0);
const Ink_Bignum_Integer Ink_Bignum_Integer::One = Ink_Bignum_Integer(1);
const Ink_Bignum_Integer Ink_Bignum_Integer::Ten = Ink_Bignum_Integer(10);

const Ink_Bignum_NumericValue Ink_Bignum_NumericValue::Zero = Ink_Bignum_NumericValue(0);
const Ink_Bignum_NumericValue Ink_Bignum_NumericValue::One = Ink_Bignum_NumericValue(1);
const Ink_Bignum_NumericValue Ink_Bignum_NumericValue::Ten = Ink_Bignum_NumericValue(10);

Ink_Bignum_Integer::Ink_Bignum_Integer()
{
	sign = true; 
}

Ink_Bignum_Integer::Ink_Bignum_Integer(double val)
{
	if (val >= 0) sign = true;
	else {
		sign = false;
		val *= -1;
	}
	val = floor(val);
	do {
		digits.push_back((char)(fmod(val, 10)));
		val /= 10;
	} while (fabs(val) >= 1);
}

Ink_Bignum_Integer::Ink_Bignum_Integer(string def)
{
	sign = true;
	for (string::reverse_iterator iter = def.rbegin();
		 iter < def.rend(); iter++) {
		char ch = *iter;
		if (iter == def.rend() - 1) {
			if (ch == '+') break;
			if (ch == '-') {
				sign = false;
				break;
			}
		}
		if ((*iter) > '9' || (*iter) < '0') {
			digits = Ink_Bignum_Integer_Digit();
			return;
		}
		digits.push_back((char)((*iter) - '0'));
	}
	trim();
}

void Ink_Bignum_Integer::trim()
{
	Ink_Bignum_Integer_Digit::reverse_iterator iter = digits.rbegin();
	while (!digits.empty() && *iter == 0) {
		digits.pop_back();
		iter = digits.rbegin();
	}
	if (!digits.size()) {
		sign = true;
		digits.push_back(0);
	}
}

Ink_Bignum_Integer::Ink_Bignum_Integer(const Ink_Bignum_Integer &op2)
{
	sign = op2.sign;
	digits = op2.digits;
}

Ink_Bignum_Integer Ink_Bignum_Integer::operator = (const Ink_Bignum_Integer &op2)
{
	digits = op2.digits;
	sign = op2.sign;
	return *this;
}

Ink_Bignum_Integer Ink_Bignum_Integer::abs() const
{
	if (sign) return *this;
	else return -(*this);
}

Ink_Bignum_Integer Ink_Bignum_Integer::pow(int a) 
{
	Ink_Bignum_Integer ret(1);
	int i;

	if (a >= 0)
		for(i = 0; i < a; i++) ret *= (*this);
	else
		for(i = 0; i > a; i--) ret /= (*this);

	return ret;
}

Ink_Bignum_Integer Ink_Bignum_Integer::pow(Ink_Bignum_Integer a)
{
	Ink_Bignum_Integer ret(1);
	Ink_Bignum_Integer i;

	if (a >= 0)
		for(i = 0; i < a; i++) ret *= (*this);
	else
		for(i = 0; i > a; i--) ret /= (*this);

	return ret;
}

Ink_Bignum_Integer Ink_Bignum_Integer::exp(long a)
{
	Ink_Bignum_Integer ret = *this;

	if (!a) return ret;

	if (a > 0) {
		Ink_Bignum_Integer_Digit ins = Ink_Bignum_Integer_Digit(a, 0);
		ret.digits.insert(ret.digits.begin(), ins.begin(), ins.end());
	} else {
		if (-a >= (long)ret.digits.size()) return 0;
		ret.digits = Ink_Bignum_Integer_Digit(ret.digits.begin() - a, ret.digits.end());
	}
	ret.trim();

	return ret;
}

Ink_Bignum_Integer operator += (Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	if (op1.sign == op2.sign) {
		Ink_Bignum_Integer_Digit::iterator iter1;
		Ink_Bignum_Integer_Digit::const_iterator iter2;
		char to_add;

		for (to_add = 0,
			 iter1 = op1.digits.begin(),
			 iter2 = op2.digits.begin();
			 iter1 != op1.digits.end()
			 && iter2 != op2.digits.end();
			 iter1++, iter2++) {
			*iter1 = *iter1 + *iter2 + to_add;
			to_add = (*iter1 > 9);
			*iter1 = *iter1 % 10;
		}

		while (iter1 != op1.digits.end()) {
			*iter1 = *iter1 + to_add;
			to_add = (*iter1 > 9);
			*iter1 %= 10;
			iter1++;
		}

		while (iter2 != op2.digits.end()) {
			char val = *iter2 + to_add;
			to_add = (val > 9);
			val %= 10;
			op1.digits.push_back(val);
			iter2++;
		}

		if (to_add != 0) op1.digits.push_back(to_add);

		return op1;
	}
	
	if (op1.sign) return op1 -= (-op2);
	
	return op1 = op2 - (-op1);
}

Ink_Bignum_Integer operator -= (Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	if ( op1.sign == op2.sign ) {
		if(op1.sign) { 
			if(op1 < op2) return op1 = -(op2 - op1);
		} else {
			if(-op1 > -op2) return op1 = -((-op1) - (-op2));
			else return op1 = (-op2) - (-op1);
		}

		Ink_Bignum_Integer_Digit::iterator iter1;
		Ink_Bignum_Integer_Digit::const_iterator iter2;
		char to_substract;

		for (to_substract = 0,
			 iter1 = op1.digits.begin(),
			 iter2 = op2.digits.begin();
			 iter1 != op1.digits.end()
			 && iter2 != op2.digits.end();
			 iter1++, iter2++) {
			*iter1 = *iter1 - *iter2 - to_substract;
			to_substract = 0;
			if (*iter1 < 0) {
				to_substract = 1;
				*iter1 += 10;
			}
		}

		while (iter1 != op1.digits.end()) {
			*iter1 = *iter1 - to_substract;
			to_substract = 0;
			if (*iter1 < 0) {
				to_substract = 1;
				*iter1 += 10;
			} else break;
			iter1++;
		}

		op1.trim();
		return op1;
	}
	
	if (op1 > Ink_Bignum_Integer::Zero) return op1 += (-op2);
	
	return op1 = -(op2 + (-op1));
}

Ink_Bignum_Integer operator *= (Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	Ink_Bignum_Integer result(0);

	if (op1 == Ink_Bignum_Integer::Zero || op2 == Ink_Bignum_Integer::Zero) result = Ink_Bignum_Integer::Zero;
	else if (op1 == Ink_Bignum_Integer::One) {
		result = op2;
	} else if (op1 == -Ink_Bignum_Integer::One) {
		result = -op2;
	} else if (op2 == Ink_Bignum_Integer::One) {
		result = op1;
	} else if (op2 == -Ink_Bignum_Integer::One) {
		result = -op1;
	} else {
		if (op1.digits.size() <= 7 && op2.digits.size() <= 7) {
			Ink_Bignum_Integer op2_c = op2;
			result = op1.toLong() * op2_c.toLong();
		} else if (op1.digits.size() <= 140 || op2.digits.size() <= 140) {
			Ink_Bignum_Integer_Digit::const_iterator iter2 = op2.digits.begin();
			while (iter2 != op2.digits.end()) {
				if (*iter2 != 0) {
					Ink_Bignum_Integer_Digit temp = op1.digits;
					char to_add;
					Ink_Bignum_Integer_Digit::iterator iter1;

					for (to_add = 0, iter1 = temp.begin();
						 iter1 != temp.end(); iter1++) {
						*iter1 *= *iter2;
						*iter1 += to_add;
						to_add = *iter1 / 10;
						*iter1 %= 10;
					}

					if (to_add != 0) temp.push_back(to_add);

					Ink_Bignum_Integer_Digit::size_type num_of_zeros = iter2 - op2.digits.begin();

					// while (num_of_zeros--) temp.push_front(0);
					Ink_Bignum_Integer_Digit add_front = Ink_Bignum_Integer_Digit(num_of_zeros, 0);

					Ink_Bignum_Integer temp2;
					temp2.digits.insert(temp2.digits.end(),
										add_front.begin(), add_front.end());
					temp2.digits.insert(temp2.digits.end(),
										temp.begin(), temp.end());
					temp2.trim();
					result = result + temp2;
				}
				iter2++;
			}
		} else {
			Ink_Bignum_Integer x = op1, y = op2;
			Ink_Bignum_Integer a, b, c, d;
			Ink_Bignum_Integer_Digit::size_type len;
			long zero_c = 0;

			if (x.digits.size() < y.digits.size()) {
				x = x.exp(zero_c = y.digits.size() - x.digits.size());
			} else if (x.digits.size() > y.digits.size()) {
				y = y.exp(zero_c = x.digits.size() - y.digits.size());
			}

			if (x.digits.size() % 2 != 0)
				x.digits.push_back(0);
			len = x.digits.size();
			b = Ink_Bignum_Integer_Digit(x.digits.begin(), x.digits.begin() + (len / 2));
			a = Ink_Bignum_Integer_Digit(x.digits.begin() + (len / 2), x.digits.end());

			if (y.digits.size() % 2 != 0)
				y.digits.push_back(0);
			len = y.digits.size();
			d = Ink_Bignum_Integer_Digit(y.digits.begin(), y.digits.begin() + (len / 2));
			c = Ink_Bignum_Integer_Digit(y.digits.begin() + (len / 2), y.digits.end());

			Ink_Bignum_Integer_Digit::size_type n = x.digits.size() > y.digits.size()
													? x.digits.size()
													: y.digits.size();
			a.trim();
			b.trim();
			c.trim();
			d.trim();

			Ink_Bignum_Integer ac = a * c;
			Ink_Bignum_Integer bd = b * d;
			Ink_Bignum_Integer t1 = (a - b) * (d - c);
			Ink_Bignum_Integer t2 = t1 + ac + bd;
			result = ac.exp(n) + t2.exp(n / 2) + bd;
			result.trim();
			result = Ink_Bignum_Integer_Digit(result.digits.begin() + zero_c, result.digits.end());
		}
		result.sign = ((op1.sign && op2.sign) || (!op1.sign && !op2.sign));
	}
	op1 = result;

	return op1;
}

Ink_Bignum_Integer operator /= (Ink_Bignum_Integer &op1 , const Ink_Bignum_Integer &op2) {
	// if( op2 == Ink_Bignum_Integer::ZERO )
	// throw DividedByZeroException();
	
	if (op2 == Ink_Bignum_Integer::One)
		return op1;
	else if (op2 == -Ink_Bignum_Integer::One) {
		op1.sign = !op1.sign;
		return op1;
	}

	Ink_Bignum_Integer t1 = op1.abs(), t2 = op2.abs();
	if (t1 < t2) {
		op1 = Ink_Bignum_Integer::Zero;
		return op1;
	}

	// t1 > t2 > 0
	deque<char> temp;
	Ink_Bignum_Integer_Digit::reverse_iterator iter = t1.digits.rbegin();

	Ink_Bignum_Integer temp2(0);
	while (iter != t1.digits.rend()) {
		temp2 = temp2 * Ink_Bignum_Integer::Ten + Ink_Bignum_Integer((int)(*iter));
		char s = 0;
		while (temp2 >= t2) {
			temp2 = temp2 - t2;
			s = s + 1;
		}
		temp.push_front(s);
		iter++;
	}
	op1.digits.clear();
	op1.digits.insert(op1.digits.end(), temp.begin(), temp.end());
	op1.trim();
	op1.sign = ((op1.sign && op2.sign) || (!op1.sign && !op2.sign));

	return op1;
}

Ink_Bignum_Integer operator %= (Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	return op1 -= ((op1 / op2) * op2);
}

Ink_Bignum_Integer operator + (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	Ink_Bignum_Integer temp(op1);
	temp += op2;
	return temp;
}

Ink_Bignum_Integer operator - (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	Ink_Bignum_Integer temp(op1);
	temp -= op2;
	return temp;
}

Ink_Bignum_Integer operator * (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	Ink_Bignum_Integer temp(op1);
	temp *= op2;
	return temp;
}

Ink_Bignum_Integer operator / (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	Ink_Bignum_Integer temp(op1);
	temp /= op2;
	return temp;
}

Ink_Bignum_Integer operator % (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	Ink_Bignum_Integer temp(op1);
	temp %= op2;
	return temp;
}

Ink_Bignum_Integer operator - (const Ink_Bignum_Integer &op) {
	Ink_Bignum_Integer temp = Ink_Bignum_Integer(op);
	temp.sign = !temp.sign;
	return temp;
}

Ink_Bignum_Integer operator ++ (Ink_Bignum_Integer &op) {
	op += Ink_Bignum_Integer::One;
	return op;
}

Ink_Bignum_Integer operator ++ (Ink_Bignum_Integer &op, int x) {
	Ink_Bignum_Integer temp(op);
	++op;
	return temp;
}

Ink_Bignum_Integer operator -- (Ink_Bignum_Integer &op) {
	op -= Ink_Bignum_Integer::One;
	return op;
}

Ink_Bignum_Integer operator -- (Ink_Bignum_Integer& op, int x) {
	Ink_Bignum_Integer temp(op);
	--op;
	return temp;
}

bool operator < (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	if (op1.sign != op2.sign)
		return !op1.sign;
	
	if (op1.digits.size() != op2.digits.size())
		return (op1.sign && op1.digits.size() < op2.digits.size())
			   || (!op1.sign && op1.digits.size() > op2.digits.size());

	Ink_Bignum_Integer_Digit::const_reverse_iterator iter1, iter2;

	iter1 = op1.digits.rbegin();
	iter2 = op2.digits.rbegin();

	while (iter1 != op1.digits.rend()) {
		if (op1.sign && *iter1 < *iter2) return true;
		if (op1.sign && *iter1 > *iter2) return false;
		if (!op1.sign && *iter1 > *iter2) return true;
		if (!op1.sign && *iter1 < *iter2) return false;
		iter1++;
		iter2++;
	}

	return false;
}

bool operator == (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	if(op1.sign != op2.sign
	   || op1.digits.size() != op2.digits.size())
		return false;

	Ink_Bignum_Integer_Digit::const_iterator iter1,iter2;
	iter1 = op1.digits.begin();
	iter2 = op2.digits.begin();

	while (iter1!= op1.digits.end()) {
		if (*iter1 != *iter2) return false;
		iter1++;
		iter2++;
	}

	return true;
}

bool operator != (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	return !(op1 == op2);
}

bool operator >= (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	return (op1 > op2) || (op1 == op2);
}

bool operator <= (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	return (op1 < op2) || (op1 == op2);
}

bool operator > (const Ink_Bignum_Integer &op1, const Ink_Bignum_Integer &op2) {
	return !(op1 <= op2);
}

// Big Numeric

Ink_Bignum_NumericValue::Ink_Bignum_NumericValue(double val)
{
	num = std_pow = 0;
	if (!isnan((long double)val)) {
		double tmp_val = val;
		Ink_Bignum_Integer integer(floor(tmp_val));
		Ink_Bignum_Integer decimal((tmp_val - floor(tmp_val)) * pow(10, (long)DEFAULT_ACC));
		num = integer.exp((long)DEFAULT_ACC) + decimal;
		//std::cout << integer << decimal << endl;
		std_pow = fabs(tmp_val) >= 1
				  ? integer.digits.size()
				  : decimal.digits.size() - DEFAULT_ACC;
	}
}

Ink_Bignum_NumericValue::Ink_Bignum_NumericValue(string str)
{
	string::size_type pos, epos;
	num = std_pow = 0;
	int sign = 1;
	Ink_Bignum_Integer integer = 0, decimal;
	Ink_Bignum_Integer expn = 0;
	bool use_e = false;

	if (str.length()) {
		if (str[0] == '-') {
			str = str.substr(1);
			sign = -1;
		} else if (str[0] == '+') str = str.substr(1);

		if (((epos = str.find_first_of('e')) != string::npos)
			|| ((epos = str.find_first_of('E')) != string::npos)) {
			use_e = true;
			if (str.substr(epos + 1).find_first_of('.') != string::npos) {
				fprintf(stderr, "Exponent must be integer\n");
				return;
			}
			expn = str.substr(epos + 1);
			str = str.substr(0, epos);
		}

		pos = str.find_first_of('.');
		if (pos != string::npos) {
			integer = Ink_Bignum_Integer(str.substr(0, pos));
			decimal = Ink_Bignum_Integer(str.substr(pos + 1)).abs();
			
			num = integer;
			num.digits.insert(num.digits.begin(),
							  decimal.digits.begin(),
							  decimal.digits.end());
			num.sign = sign > 0;

			std_pow = integer > 0 ? integer.digits.size() : num.digits.size() - decimal.digits.size();
		} else {
			num = sign * Ink_Bignum_Integer(str);
			std_pow = num.digits.size();
		}
	}

	if (!num.isValid()) {
		*this = INVALID_NUM;
		return;
	}

	if (use_e) {
		long tmp = expn.toLong();
		std_pow += tmp;
		num = num.exp(tmp);
	}
}

Ink_Bignum_NumericValue::Ink_Bignum_NumericValue(Ink_Bignum_Integer int_val)
{
    num = int_val;
    std_pow = int_val.digits.size();
}

Ink_Bignum_NumericValue operator += (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs)
{
	Ink_Bignum_Integer tmp_lhs;
	Ink_Bignum_Integer tmp_rhs;
	Ink_Bignum_Integer ten = Ink_Bignum_Integer::Ten;
	long sup = lhs.getDecimal() - rhs.getDecimal();

	if (sup > 0) {
		tmp_lhs = lhs.num;
		tmp_rhs = rhs.num * ten.pow(abs(sup));
	} else {
		tmp_lhs = lhs.num * ten.pow(abs(sup));
		tmp_rhs = rhs.num;
	}

	Ink_Bignum_NumericValue ret = Ink_Bignum_NumericValue(tmp_lhs + tmp_rhs);
	ret.std_pow = max(lhs.std_pow, rhs.std_pow) + (ret.num.digits.size() - max(tmp_lhs.digits.size(), tmp_rhs.digits.size()));
	lhs = ret;

	return ret;
}

Ink_Bignum_NumericValue operator -= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs)
{
	Ink_Bignum_Integer tmp_lhs;
	Ink_Bignum_Integer tmp_rhs;
	Ink_Bignum_Integer ten = Ink_Bignum_Integer::Ten;
	long sup = lhs.getDecimal() - rhs.getDecimal();

	if (sup > 0) {
		tmp_lhs = lhs.num;
		tmp_rhs = rhs.num * ten.pow(abs(sup));
	} else {
		tmp_lhs = lhs.num * ten.pow(abs(sup));
		tmp_rhs = rhs.num;
	}

	Ink_Bignum_NumericValue ret = Ink_Bignum_NumericValue(tmp_lhs - tmp_rhs);
	ret.std_pow = max(lhs.std_pow, rhs.std_pow) + (ret.num.digits.size() - max(tmp_lhs.digits.size(), tmp_rhs.digits.size()));
	lhs = ret;

	return ret;
}

Ink_Bignum_NumericValue operator *= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs)
{
	Ink_Bignum_NumericValue ret = Ink_Bignum_NumericValue(lhs.num * rhs.num);
	ret.std_pow = ret.num.digits.size() - (lhs.getDecimal() + rhs.getDecimal());
	lhs = ret;

	return ret;
}

Ink_Bignum_NumericValue operator /= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs)
{
	return lhs = lhs.dividedBy(rhs);
}

Ink_Bignum_NumericValue operator %= (Ink_Bignum_NumericValue &lhs, const Ink_Bignum_NumericValue &rhs)
{
	Ink_Bignum_Integer tmp_lhs;
	Ink_Bignum_Integer tmp_rhs;
	Ink_Bignum_Integer ten = Ink_Bignum_Integer::Ten;
	long sup = lhs.getDecimal() - rhs.getDecimal();

	if (sup > 0) {
		tmp_lhs = lhs.num;
		tmp_rhs = rhs.num * ten.pow(abs(sup));
	} else {
		tmp_lhs = lhs.num * ten.pow(abs(sup));
		tmp_rhs = rhs.num;
	}
	Ink_Bignum_NumericValue ret;
	ret.num = tmp_lhs - (tmp_lhs / tmp_rhs * tmp_rhs);
	ret.std_pow =  ret.num.digits.size() - max(lhs.getDecimal(), rhs.getDecimal());
	lhs = ret;

	return ret;
}

Ink_Bignum_NumericValue operator + (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	Ink_Bignum_NumericValue temp(op1);
	temp += op2;
	return temp;
}

Ink_Bignum_NumericValue operator - (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	Ink_Bignum_NumericValue temp(op1);
	temp -= op2;
	return temp;
}

Ink_Bignum_NumericValue operator * (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	Ink_Bignum_NumericValue temp(op1);
	temp *= op2;
	return temp;
}

Ink_Bignum_NumericValue operator / (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	Ink_Bignum_NumericValue temp(op1);
	temp /= op2;
	return temp;
}

Ink_Bignum_NumericValue operator % (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	Ink_Bignum_NumericValue temp(op1);
	temp %= op2;
	return temp;
}

Ink_Bignum_NumericValue operator - (const Ink_Bignum_NumericValue &op)
{
	Ink_Bignum_NumericValue temp(op);
	temp.num = -temp.num;
	return temp;
}

Ink_Bignum_NumericValue operator + (const Ink_Bignum_NumericValue &op)
{
	return op;
}

Ink_Bignum_NumericValue operator ++ (Ink_Bignum_NumericValue &op) // ++v
{
	op += Ink_Bignum_NumericValue::One;
	return op;
}

Ink_Bignum_NumericValue operator -- (Ink_Bignum_NumericValue &op) // --v
{
	op -= Ink_Bignum_NumericValue::One;
	return op;
}

Ink_Bignum_NumericValue operator ++ (Ink_Bignum_NumericValue &op, int a) // v++
{
	Ink_Bignum_NumericValue temp(op);
	op += Ink_Bignum_NumericValue::One;
	return temp;
}

Ink_Bignum_NumericValue operator -- (Ink_Bignum_NumericValue &op, int a) // v--
{
	Ink_Bignum_NumericValue temp(op);
	op -= Ink_Bignum_NumericValue::One;
	return temp;
}

bool operator < (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	if (op1.std_pow != op2.std_pow)
		return op1.std_pow < op2.std_pow;

	long sup = op1.getDecimal() - op2.getDecimal();
	Ink_Bignum_Integer ten = Ink_Bignum_Integer::Ten;
	Ink_Bignum_Integer tmp_op1;
	Ink_Bignum_Integer tmp_op2;

	if (sup > 0) {
		tmp_op1 = op1.num;
		tmp_op2 = op2.num * ten.pow(abs(sup));
	} else {
		tmp_op1 = op1.num * ten.pow(abs(sup));
		tmp_op2 = op2.num;
	}

	return tmp_op1 < tmp_op2;
}

bool operator == (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	long sup = op1.getDecimal() - op2.getDecimal();
	Ink_Bignum_Integer ten = Ink_Bignum_Integer::Ten;
	Ink_Bignum_Integer tmp_op1;
	Ink_Bignum_Integer tmp_op2;

	if (sup > 0) {
		tmp_op1 = op1.num;
		tmp_op2 = op2.num * ten.pow(abs(sup));
	} else {
		tmp_op1 = op1.num * ten.pow(abs(sup));
		tmp_op2 = op2.num;
	}

	return op1.std_pow == op2.std_pow && tmp_op1 == tmp_op2;
}

bool operator != (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	return !(op1 == op2);
}

bool operator > (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	return !(op1 < op2 || op1 == op2);
}

bool operator <= (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	return op1 < op2 || op1 == op2;
}

bool operator >= (const Ink_Bignum_NumericValue &op1, const Ink_Bignum_NumericValue &op2)
{
	return !(op1 < op2);
}
