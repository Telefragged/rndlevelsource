#pragma once

#include <unordered_map>

class KeyValBase
{
private:
	mutable size_t depth_;
public:
	std::unordered_map<std::string, std::string> keyvals;
	KeyValBase();
	~KeyValBase();

	size_t depth() const;
	void depth(size_t) const;

	std::string& operator[](std::string);
	std::string operator[](std::string) const;

	std::string get(std::string) const;

	static std::string toStr(const std::pair<std::string, std::string>&, char enclose = '\"');
};

