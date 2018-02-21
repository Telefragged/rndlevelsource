#pragma once

#include <unordered_map>

class KeyValBase
{
private:
	mutable size_t depth_ = 0;
public:
	std::unordered_map<std::string, std::string> keyvals;
	KeyValBase();
	~KeyValBase();

	size_t depth() const;
	void depth(size_t depth) const;

	std::string& operator[](const std::string& key);
	std::string operator[](const std::string& key) const;

	std::string get(const std::string& key) const;

	static std::string toStr(const std::pair<std::string, std::string>& pair, char enclose = '\"');
};

