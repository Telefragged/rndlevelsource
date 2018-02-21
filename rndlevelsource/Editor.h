#pragma once
#include <iostream>
#include <iomanip>
#include <vector>

#include "KeyVal.h"

class Editor
{
private:
	mutable size_t depth_ = 0;
public:
	std::vector<KeyVal> keyvals;
	unsigned int parse(std::istream& stream);

	size_t depth() const
	{
		return depth_;
	}

	void depth(size_t newDepth) const
	{
		depth_ = newDepth;
	}

	friend std::ostream& operator<<(std::ostream& os, const Editor& e);

	Editor() = default;
	~Editor() = default;
};

inline std::ostream& operator<<(std::ostream& os, const Editor& e)
{
	if (e.keyvals.empty())
		return os;

	os << std::setw(e.depth()) << "" << "editor\n";
	os << std::setw(e.depth()) << "" << "{\n";
	for (const KeyVal& kv : e.keyvals)
	{
		os << std::setw(e.depth()) << "" << "\t" << kv.toStr() << "\n";
	}
	os << std::setw(e.depth()) << "" << "}";
	return os;
}

