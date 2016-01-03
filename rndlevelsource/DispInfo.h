#pragma once

#include <vector>
#include <ostream>
#include <iomanip>
#include "Vertex.h"
#include "KeyValBase.h"

#define TABDEPTH 1U

class SingleDisp
{
public:

	Vertex normal;
	double distance;
	Vertex offset;
	Vertex offset_normal;
	char alpha;

	SingleDisp();
};

class DispInfo :
	public KeyValBase
{
	mutable unsigned int depth_;
public:
	unsigned int parse(std::istream&);
	std::vector<std::vector<SingleDisp>> info;

	unsigned int depth() const
	{
		return depth_;
	}

	void depth(unsigned int newDepth) const
	{
		depth_ = newDepth;
	}

	void setHeight(int x, int y, double height);

	void rotate(const Vertex&, const Matrix3d&);

	friend std::ostream& operator<<(std::ostream&, const DispInfo&);

	DispInfo();
};

inline std::ostream& operator<<(std::ostream& os, const DispInfo& d)
{
	if (d.keyvals.size() == 0) return os;
	os << std::setw(d.depth()) << "" << "dispinfo\n";
	os << std::setw(d.depth()) << "" << "{\n";
	for (const auto& pair : d.keyvals)
	{
		os << std::setw(d.depth()) << "" << "\t" << KeyValBase::toStr(pair) << "\n";
	}

	int sz = (int)pow(2, atoi(d["power"].c_str())) + 1;

	os << std::setw(d.depth()) << "" << "\tnormals\n";
	os << std::setw(d.depth()) << "" << "\t{\n";
	for (int n = 0; n < sz; n++)
	{
		os << std::setw(d.depth()) << "" << "\t\t";
		os << "\"row";
		os << std::to_string(n) + "\" \"";
		bool first = true;
		for (auto& disp : d.info[n])
		{
			if (!first) os << ' ';
			os << disp.normal.toStr();
			first = false;
		}
		os << "\"\n";
	}
	os << std::setw(d.depth()) << "" << "\t}\n";

	os << std::setw(d.depth()) << "" << "\tdistances\n";
	os << std::setw(d.depth()) << "" << "\t{\n";
	for (int n = 0; n < sz; n++)
	{
		os << std::setw(d.depth()) << "" << "\t\t";
		os << "\"row";
		os << std::to_string(n) + "\" \"";
		bool first = true;
		for (auto& disp : d.info[n])
		{
			if (!first) os << ' ';
			os << disp.distance;
			first = false;
		}
		os << "\"\n";
	}
	os << std::setw(d.depth()) << "" << "\t}\n";

	os << std::setw(d.depth()) << "" << "\toffsets\n";
	os << std::setw(d.depth()) << "" << "\t{\n";
	for (int n = 0; n < sz; n++)
	{
		os << std::setw(d.depth()) << "" << "\t\t";
		os << "\"row";
		os << std::to_string(n) + "\" \"";
		bool first = true;
		for (auto& disp : d.info[n])
		{
			if (!first) os << ' ';
			os << disp.offset.toStr();
			first = false;
		}
		os << "\"\n";
	}
	os << std::setw(d.depth()) << "" << "\t}\n";

	os << std::setw(d.depth()) << "" << "\toffset_normals\n";
	os << std::setw(d.depth()) << "" << "\t{\n";
	for (int n = 0; n < sz; n++)
	{
		os << std::setw(d.depth()) << "" << "\t\t";
		os << "\"row";
		os << std::to_string(n) + "\" \"";
		bool first = true;
		for (auto& disp : d.info[n])
		{
			if (!first) os << ' ';
			os << disp.offset_normal.toStr();
			first = false;
		}
		os << "\"\n";
	}
	os << std::setw(d.depth()) << "" << "\t}\n";

	os << std::setw(d.depth()) << "" << "\talphas\n";
	os << std::setw(d.depth()) << "" << "\t{\n";
	for (int n = 0; n < sz; n++)
	{
		os << std::setw(d.depth()) << "" << "\t\t";
		os << "\"row";
		os << std::to_string(n) + "\" \"";
		bool first = true;
		for (auto& disp : d.info[n])
		{
			if (!first) os << ' ';
			os << (int)disp.alpha;
			first = false;
		}
		os << "\"\n";
	}
	os << std::setw(d.depth()) << "" << "\t}\n";

	os << std::setw(d.depth()) << "" << "}";
	return os;
}

