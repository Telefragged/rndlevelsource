#pragma once

#include <unordered_map>

class KeyValBase
{
protected:
	std::unordered_map<std::string, std::string> keyvals;

public:
	KeyValBase();
	~KeyValBase();

	std::string & operator[](std::string);
	std::string operator[](std::string) const;

	std::string get(std::string) const;

	static std::string toStr(const std::pair <std::string, std::string> &, char enclose = '\"');

};
