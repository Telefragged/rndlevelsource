#pragma once

#include <string>

class KeyVal
{
	std::string key_;
	std::string val_;
public:

	std::string toStr(char enclose = '\"') const;
	int toInt();
	double toDouble();

	void key(const std::string &key);
	void val(const std::string &val);

	const std::string& key() const;
	const std::string& val() const;

	// void fromInt(int);
	// void fromDouble(double);

	static bool keycmp(const KeyVal& lhs, const std::string& rhs);
	//static bool keyregex(const KeyVal&, const std::string&);

	KeyVal() = default;
	KeyVal(const std::string& str);
	KeyVal(const std::string& k, const std::string& v);
	~KeyVal() = default;
};

