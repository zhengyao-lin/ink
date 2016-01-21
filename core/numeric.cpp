#include "numeric.h"

using namespace std;

const Ink_BigInteger Ink_BigInteger::Zero = Ink_BigInteger(0);
const Ink_BigInteger Ink_BigInteger::One = Ink_BigInteger(1);
const Ink_BigInteger Ink_BigInteger::Ten = Ink_BigInteger(10);

Ink_BigInteger::Ink_BigInteger()
{
	sign = true; 
}

Ink_BigInteger::Ink_BigInteger(long val)
{
	if (val >= 0) sign = true;
	else {
		sign = false;
		val *= -1;
	}
	do {
		digits.push_back((char)(val % 10));
		val /= 10;
	} while (val != 0);
}

Ink_BigInteger::Ink_BigInteger(string def)
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
		digits.push_back((char)((*iter) - '0'));
	}
	trim();
}

void Ink_BigInteger::trim()
{
	vector<char>::reverse_iterator iter = digits.rbegin();
	while (!digits.empty() && *iter == 0) {
		digits.pop_back();
		iter = digits.rbegin();
	}
	if (!digits.size()) {
		sign = true;
		digits.push_back(0);
	}
}

Ink_BigInteger::Ink_BigInteger(const Ink_BigInteger &op2)
{
	sign = op2.sign;
	digits = op2.digits;
}

Ink_BigInteger Ink_BigInteger::operator = (const Ink_BigInteger &op2)
{
	digits = op2.digits;
	sign = op2.sign;
	return *this;
}

Ink_BigInteger Ink_BigInteger::abs() const
{
	if (sign) return *this;
	else return -(*this);
}

Ink_BigInteger Ink_BigInteger::pow(int a) 
{
	Ink_BigInteger ret(1);
	int i;

	for(i = 0; i < a; i++) ret *= (*this);

	return ret;
}

Ink_BigInteger Ink_BigInteger::pow(Ink_BigInteger a)
{
	Ink_BigInteger ret(1);
	Ink_BigInteger i;

	for(i = 0; i < a; i++) ret *= (*this);

	return ret;
}

Ink_BigInteger operator += (Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	if (op1.sign == op2.sign) {
		vector<char>::iterator iter1;
		vector<char>::const_iterator iter2;
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

Ink_BigInteger operator -= (Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	if ( op1.sign == op2.sign ) {
		if(op1.sign) { 
			if(op1 < op2) return op1 = -(op2 - op1);
		} else {
			if(-op1 > -op2) return op1 = -((-op1) - (-op2));
			else return op1 = (-op2) - (-op1);
		}

		vector<char>::iterator iter1;
		vector<char>::const_iterator iter2;
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
	
	if (op1 > Ink_BigInteger::Zero) return op1 += (-op2);
	
	return op1 = -(op2 + (-op1));
}

Ink_BigInteger operator *= (Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	Ink_BigInteger result(0);

	if (op1 == Ink_BigInteger::Zero || op2 == Ink_BigInteger::Zero) result = Ink_BigInteger::Zero;
	else {
		vector<char>::const_iterator iter2 = op2.digits.begin();
		while (iter2 != op2.digits.end()) {
			if (*iter2 != 0) {
				deque<char> temp(op1.digits.begin(), op1.digits.end());
				char to_add;
				deque<char>::iterator iter1;

				for (to_add = 0, iter1 = temp.begin();
					 iter1 != temp.end(); iter1++) {
					*iter1 *= *iter2;
					*iter1 += to_add;
					to_add = *iter1 / 10;
					*iter1 %= 10;
				}

				if (to_add != 0) temp.push_back(to_add);

				int num_of_zeros = iter2 - op2.digits.begin();

				while (num_of_zeros--) temp.push_front(0);

				Ink_BigInteger temp2;
				temp2.digits.insert(temp2.digits.end(),
									temp.begin(), temp.end());
				temp2.trim();
				result = result + temp2;
			}
			iter2++;
		}
		result.sign = ((op1.sign && op2.sign) || (!op1.sign && !op2.sign));
	}
	op1 = result;

	return op1;
}

Ink_BigInteger operator /= (Ink_BigInteger &op1 , const Ink_BigInteger &op2) {
	// if( op2 == Ink_BigInteger::ZERO )
	// throw DividedByZeroException();
	Ink_BigInteger t1 = op1.abs(), t2 = op2.abs();
	if (t1 < t2) {
		op1 = Ink_BigInteger::Zero;
		return op1;
	}

	// t1 > t2 > 0
	deque<char> temp;
	vector<char>::reverse_iterator iter = t1.digits.rbegin();

	Ink_BigInteger temp2(0);
	while (iter != t1.digits.rend()) {
		temp2 = temp2 * Ink_BigInteger::Ten + Ink_BigInteger((int)(*iter));
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

Ink_BigInteger operator %= (Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	return op1 -= ((op1 / op2) * op2);
}

Ink_BigInteger operator + (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	Ink_BigInteger temp(op1);
	temp += op2;
	return temp;
}

Ink_BigInteger operator - (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	Ink_BigInteger temp(op1);
	temp -= op2;
	return temp;
}

Ink_BigInteger operator * (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	Ink_BigInteger temp(op1);
	temp *= op2;
	return temp;
}

Ink_BigInteger operator / (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	Ink_BigInteger temp(op1);
	temp /= op2;
	return temp;
}

Ink_BigInteger operator % (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	Ink_BigInteger temp(op1);
	temp %= op2;
	return temp;
}

Ink_BigInteger operator - (const Ink_BigInteger &op) {
	Ink_BigInteger temp = Ink_BigInteger(op);
	temp.sign = !temp.sign;
	return temp;
}

Ink_BigInteger operator ++ (Ink_BigInteger &op) {
	op += Ink_BigInteger::One;
	return op;
}

Ink_BigInteger operator ++ (Ink_BigInteger &op, int x) {
	Ink_BigInteger temp(op);
	++op;
	return temp;
}

Ink_BigInteger operator -- (Ink_BigInteger &op) {
	op -= Ink_BigInteger::One;
	return op;
}

Ink_BigInteger operator -- (Ink_BigInteger& op, int x) {
	Ink_BigInteger temp(op);
	--op;
	return temp;
}

bool operator < (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	if (op1.sign != op2.sign)
		return !op1.sign;
	
	if (op1.digits.size() != op2.digits.size())
		return (op1.sign && op1.digits.size() < op2.digits.size())
			   || (!op1.sign && op1.digits.size() > op2.digits.size());

	vector<char>::const_reverse_iterator iter1, iter2;

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

bool operator == (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	if(op1.sign != op2.sign
	   || op1.digits.size() != op2.digits.size())
		return false;

	vector<char>::const_iterator iter1,iter2;
	iter1 = op1.digits.begin();
	iter2 = op2.digits.begin();

	while (iter1!= op1.digits.end()) {
		if (*iter1 != *iter2) return false;
		iter1++;
		iter2++;
	}

	return true;
}

bool operator != (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	return !(op1 == op2);
}

bool operator >= (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	return (op1 > op2) || (op1 == op2);
}

bool operator <= (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	return (op1 < op2) || (op1 == op2);
}

bool operator > (const Ink_BigInteger &op1, const Ink_BigInteger &op2) {
	return !(op1 <= op2);
}

