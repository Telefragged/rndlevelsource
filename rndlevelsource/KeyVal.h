#pragma once

#include <string>

class KeyVal
{
public:
	std::string key;
	std::string val;

	std::string toStr(char enclose = '\"') const;
	int toInt();
	double toDouble();

	std::string keyc() const;
	std::string valc() const;

	// void fromInt(int);
	// void fromDouble(double);

	static bool keycmp(const KeyVal& lhs, const std::string& rhs);
	//static bool keyregex(const KeyVal&, const std::string&);

	KeyVal();
	KeyVal(const std::string& str);
	KeyVal(const std::string& k, const std::string& v);
	~KeyVal();
};

