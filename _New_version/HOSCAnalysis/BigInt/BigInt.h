/*
* Title: BigInt
* Description: Big Integer class (coded with competitive programming problems in mind)
* Date: 09-October-2017
* Author: William Chanrico
*/

#include <bits/stdc++.h>
#include <assert.h>
using namespace std;

class BigInt
{
private:
	int sign = {1};
	string s = {};

public:
	BigInt() {}

	BigInt(string x);
	BigInt(int x);
	BigInt negative();
	BigInt normalize(int newSign);
	void operator=(string x);
	bool operator==(const BigInt &x) const;
	bool operator<(const BigInt &x) const;
	bool operator<=(const BigInt &x) const;
	bool operator>(const BigInt &x) const;
	bool operator>=(const BigInt &x) const;
	BigInt operator+(BigInt x);
	BigInt operator-(BigInt x);
	BigInt operator*(BigInt x);
	BigInt operator/(BigInt x);
	BigInt operator%(BigInt x);
	// operator string(){
	// 	string ret = s;

	// 	reverse(ret.begin(), ret.end());

	// 	return (sign == -1 ? "-" : "") + s;
	// }
	string toString() const;
	BigInt toBase10(int base);
	BigInt toBase10(int base, BigInt mod);
	string convertToBase(int base);
	BigInt toBase(int base);
	friend ostream &operator<<(ostream &os, const BigInt &x);
};

bool intTest(int x, int y);
bool test();
