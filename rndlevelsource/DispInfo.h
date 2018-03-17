#pragma once

#include <vector>
#include <ostream>
#include <iomanip>
#include "Vertex.h"
#include "KeyValBase.h"

class SingleDisp
{
public:

	Vertex normal;
	double distance = 0.0;
	Vertex offset;
	Vertex offset_normal;
	char alpha = 0;

	SingleDisp();
};

class DispInfo :
	public KeyValBase
{
public:
	unsigned int parse(std::istream& stream);

	std::vector<std::vector<SingleDisp>> info;

	void setHeight(int x, int y, double height);

	void rotate(const Vertex& point, const Matrix3d& rotmat);

	std::string getName() const override;

	friend std::ostream& operator<<(std::ostream& os, const DispInfo& dispinfo);

	DispInfo() = default;
};

inline std::ostream& operator<<(std::ostream& os, const DispInfo& dispinfo)
{
	if (dispinfo.keyvals.empty())
		return os;

	os << std::setw(dispinfo.depth()) << "" << "dispinfo\n";
	os << std::setw(dispinfo.depth()) << "" << "{\n";

	for (const auto& pair : dispinfo.keyvals)
		os << std::setw(dispinfo.depth()) << "" << "\t" << KeyValBase::toStr(pair) << "\n";

	int sz = (int)pow(2, atoi(dispinfo["power"].data())) + 1;

	os << std::setw(dispinfo.depth()) << "" << "\tnormals\n";
	os << std::setw(dispinfo.depth()) << "" << "\t{\n";
	for (int n = 0; n < sz; n++)
	{
		os << std::setw(dispinfo.depth()) << "" << "\t\t";
		os << "\"row";
		os << std::to_string(n) + "\" \"";
		bool first = true;
		for (auto& disp : dispinfo.info[n])
		{
			if (!first)
				os << ' ';

			os << disp.normal.toStr();
			first = false;
		}
		os << "\"\n";
	}
	os << std::setw(dispinfo.depth()) << "" << "\t}\n";

	os << std::setw(dispinfo.depth()) << "" << "\tdistances\n";
	os << std::setw(dispinfo.depth()) << "" << "\t{\n";
	for (int n = 0; n < sz; n++)
	{
		os << std::setw(dispinfo.depth()) << "" << "\t\t";
		os << "\"row";
		os << std::to_string(n) + "\" \"";
		bool first = true;
		for (auto& disp : dispinfo.info[n])
		{
			if (!first)
				os << ' ';
			os << disp.distance;
			first = false;
		}
		os << "\"\n";
	}
	os << std::setw(dispinfo.depth()) << "" << "\t}\n";

	os << std::setw(dispinfo.depth()) << "" << "\toffsets\n";
	os << std::setw(dispinfo.depth()) << "" << "\t{\n";
	for (int n = 0; n < sz; n++)
	{
		os << std::setw(dispinfo.depth()) << "" << "\t\t";
		os << "\"row";
		os << std::to_string(n) + "\" \"";
		bool first = true;
		for (auto& disp : dispinfo.info[n])
		{
			if (!first)
				os << ' ';
			os << disp.offset.toStr();
			first = false;
		}
		os << "\"\n";
	}
	os << std::setw(dispinfo.depth()) << "" << "\t}\n";

	os << std::setw(dispinfo.depth()) << "" << "\toffset_normals\n";
	os << std::setw(dispinfo.depth()) << "" << "\t{\n";
	for (int n = 0; n < sz; n++)
	{
		os << std::setw(dispinfo.depth()) << "" << "\t\t";
		os << "\"row";
		os << std::to_string(n) + "\" \"";
		bool first = true;
		for (auto& disp : dispinfo.info[n])
		{
			if (!first)
				os << ' ';
			os << disp.offset_normal.toStr();
			first = false;
		}
		os << "\"\n";
	}
	os << std::setw(dispinfo.depth()) << "" << "\t}\n";

	os << std::setw(dispinfo.depth()) << "" << "\talphas\n";
	os << std::setw(dispinfo.depth()) << "" << "\t{\n";
	for (int n = 0; n < sz; n++)
	{
		os << std::setw(dispinfo.depth()) << "" << "\t\t";
		os << "\"row";
		os << std::to_string(n) + "\" \"";
		bool first = true;
		for (auto& disp : dispinfo.info[n])
		{
			if (!first)
				os << ' ';
			os << (int)disp.alpha;
			first = false;
		}
		os << "\"\n";
	}
	os << std::setw(dispinfo.depth()) << "" << "\t}\n";

	os << std::setw(dispinfo.depth()) << "" << "}";
	return os;
}

