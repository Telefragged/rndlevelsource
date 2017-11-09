#include "KeyValBase.h"

#include <iostream>

KeyValBase::KeyValBase() :
	depth_(0)
{
}

KeyValBase::~KeyValBase()
{
}

size_t KeyValBase::depth() const
{
	return depth_;
}

void KeyValBase::depth(size_t depth) const
{
	depth_ = depth;
}

std::string KeyValBase::operator[](std::string key) const
{
	if (keyvals.count(key) == 0) return "";
	return keyvals.at(key);
}

std::string& KeyValBase::operator[](std::string key)
{
	return keyvals[key];
}

std::string KeyValBase::get(std::string key) const
{
	return (*this)[key];
}

std::string KeyValBase::toStr(const std::pair<std::string, std::string>& pair, char enclose)
{
	return enclose + pair.first + enclose + ' ' + enclose + pair.second + enclose;
}

