#include "stdafx.h"
#include "KeyVal.h"
#include <iostream>

//#include <regex>

std::string KeyVal::toStr(char enclose) const
{
	std::string ret;
	ret += enclose + key + enclose + ' ' + enclose + val + enclose;
	return ret;
}

int KeyVal::toInt()
{
	return atoi(val.c_str());
}

double KeyVal::toDouble()
{
	return atof(val.c_str());
}

std::string KeyVal::keyc() const
{
	return key;
}

std::string KeyVal::valc() const
{
	return val;
}

// void KeyVal::fromInt(int newVal) {
// val = std::to_string(newVal);
// }

// void KeyVal::fromDouble(double newVal) {
// val = std::to_string(newVal);
// }

bool KeyVal::keycmp(const KeyVal& lhs, const std::string& rhs)
{
	return lhs.key == rhs;
}

//bool KeyVal::keyregex(const KeyVal &lhs, const std::string &rhs) {
//	std::regex reg(rhs);
//	return std::regex_match(lhs.key, reg);
//}

KeyVal::KeyVal(void)
{
}

KeyVal::KeyVal(const std::string& str)
{
	std::string trimmed = trim(str);
	size_t fsplit = trimmed.find_first_of("\"", 1);
	if (fsplit > trimmed.length()) fsplit = trimmed.length();
	std::string left = trimmed.substr(0, fsplit);
	left = trim(left, "\"");
	this->key = left;
	if (fsplit == trimmed.length()) return;
	size_t esplit = trimmed.find_first_of("\"", fsplit + 1);
	if (esplit == 0 || esplit >= trimmed.length()) esplit = fsplit;
	std::string right = trimmed.substr(esplit + 1);
	right = trim(right, "\"");
	this->val = right;
}

KeyVal::KeyVal(std::string k, std::string v) :
	key(k),
	val(v)
{
}

KeyVal::~KeyVal(void)
{
}

