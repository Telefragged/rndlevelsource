#include "KeyVal.h"

#include <iostream>

#include "utils.h"

//#include <regex>

std::string KeyVal::toStr(char enclose) const
{
	std::string ret;
	ret += enclose + key_ + enclose + ' ' + enclose + val_ + enclose;
	return ret;
}

int KeyVal::toInt()
{
	return atoi(val_.c_str());
}

double KeyVal::toDouble()
{
	return atof(val_.c_str());
}

void KeyVal::key(const std::string& key)
{
	key_ = key;
}

void KeyVal::val(const std::string& val)
{
	val_ = val;
}

const std::string& KeyVal::key() const
{
	return key_;
}

const std::string& KeyVal::val() const
{
	return val_;
}

// void KeyVal::fromInt(int newVal) {
// val = std::to_string(newVal);
// }

// void KeyVal::fromDouble(double newVal) {
// val = std::to_string(newVal);
// }

bool KeyVal::keycmp(const KeyVal& lhs, const std::string& rhs)
{
	return lhs.key() == rhs;
}

//bool KeyVal::keyregex(const KeyVal &lhs, const std::string &rhs) {
//	std::regex reg(rhs);
//	return std::regex_match(lhs.key, reg);
//}

KeyVal::KeyVal()
{
}

KeyVal::KeyVal(const std::string& str)
{
	auto trimmed = trim(str);

	auto fsplit = trimmed.find_first_of("\"", 1);
	if (fsplit > trimmed.length())
		fsplit = trimmed.length();

	auto left = trimmed.substr(0, fsplit) ;
	left = trim(left, "\"");
	this->key_ = left;

	if (fsplit == trimmed.length())
		return;

	auto esplit = trimmed.find_first_of("\"", fsplit + 1);
	if (esplit == 0 || esplit >= trimmed.length())
		esplit = fsplit;

	auto right = trimmed.substr(esplit + 1);
	right = trim(right, "\"");
	this->val_ = right;
}

KeyVal::KeyVal(const std::string& k, const std::string& v) :
	key_(k),
	val_(v)
{
}

KeyVal::~KeyVal()
{
}

