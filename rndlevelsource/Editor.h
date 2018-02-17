#pragma once
#include <iostream>
#include <iomanip>
#include "KeyVal.h"
#include "LinkedList.h"

class Editor
{
private:
	mutable size_t depth_;
public:
	LinkedList<KeyVal> keyvals;
	unsigned int parse(std::istream&);

	size_t depth() const
	{
		return depth_;
	}

	void depth(size_t newDepth) const
	{
		depth_ = newDepth;
	}

	friend std::ostream& operator<<(std::ostream&, const Editor&);

	Editor(void);
	~Editor(void);
};

inline std::ostream& operator<<(std::ostream& os, const Editor& e)
{
	if (e.keyvals.size() == 0U) return os;
	os << std::setw(e.depth()) << "" << "editor\n";
	os << std::setw(e.depth()) << "" << "{\n";
	for (const KeyVal& kv : e.keyvals)
	{
		os << std::setw(e.depth()) << "" << "\t" << kv.toStr() << "\n";
	}
	os << std::setw(e.depth()) << "" << "}";
	return os;
}

