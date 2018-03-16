#pragma once

#include <map>
#include <string>
#include <iostream>
#include <iomanip>
#include <memory>

#ifndef TABDEPTH
#define TABDEPTH 1U
#endif // TABDEPTH

class KeyValBase
{
protected:
	mutable size_t depth_ = 0;

	size_t id_ = 0;

	std::map<std::string, std::string> keyvals;
public:

	size_t depth() const;
	void depth(size_t depth) const;

	std::string& operator[](const std::string& key);
	std::string operator[](const std::string& key) const;

	std::string_view get(std::string_view key) const;

	bool hasKey(const std::string &key) const;

	static std::pair<std::string, std::string> parseKeyval(std::string_view keyval);

	static std::string toStr(const std::pair<std::string, std::string>& pair, char enclose = '\"');

	friend std::ostream& operator<<(std::ostream& os, const KeyValBase& k);

	size_t parse(std::istream& stream);

	virtual size_t parseSpecial(std::istream& stream, std::string_view type);

	virtual bool empty() const;
	
	virtual void extraOutput(std::ostream& os) const;

	virtual std::string getName() const = 0;

	KeyValBase() = default;
	KeyValBase(std::initializer_list<decltype(keyvals)::value_type> init);
	virtual ~KeyValBase() = default;
};

inline std::ostream& operator<<(std::ostream& os, const KeyValBase& k)
{
	if (k.empty() && k.id_ == 0)
		return os;

	os << std::setw(k.depth()) << "" << k.getName() << "\n";
	os << std::setw(k.depth()) << "" << "{\n";

	if(k.id_ != 0)
		os << std::setw(k.depth()) << "" << "\t\"id\" \"" << k.id_ << "\"\n";

	for (const auto& kv : k.keyvals)
	{
		os << std::setw(k.depth()) << "" << "\t" << KeyValBase::toStr(kv) << "\n";
	}

	k.extraOutput(os);
	
	os << std::setw(k.depth()) << "" << "}";
	return os;
}
