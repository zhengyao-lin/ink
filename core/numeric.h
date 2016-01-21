#ifndef _NUMERIC_H_
#define _NUMERIC_H_

#include <deque>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>

using namespace std;

class Ink_BigInteger {
private:
	vector<char> digits;
	bool sign;
	void trim();

public:
	friend class BigNumeric;
	Ink_BigInteger(int);
	Ink_BigInteger(string &);
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
};

#endif