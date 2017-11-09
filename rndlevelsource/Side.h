#pragma once

#include <ios>
#include <iomanip>
#include "Axis.h"
#include "DispInfo.h"
#include "KeyValBase.h"
#include "Plane.h"
#include "Polygon.h"

#ifndef TABDEPTH
#define TABDEPTH 1U
#endif

class BoundingBox;

class Side :
	public KeyValBase
{
private:
	unsigned int id_;
public:
	DispInfo disp;
	Axis uaxis, vaxis;
	Polygon polygon;
	//Plane p;

	unsigned int parse(std::istream&);

	unsigned int id() const
	{
		return id_;
	}

	Plane plane() const;

	BoundingBox bbox() const;

	void popuvars();
	void rotate(const Vertex&, const Matrix3d&);
	void move(const Vector&);
	void reID(unsigned int&);

	static bool testCollision(const Side&, const Side&);

	friend std::ostream& operator<<(std::ostream&, const Side& s);

	Side(void);
	~Side(void);
};

inline std::ostream& operator<<(std::ostream& os, const Side& s)
{
	os << std::setw(s.depth()) << "" << "side\n";
	os << std::setw(s.depth()) << "" << "{\n";
	os << std::setw(s.depth()) << "" << "\t\"id\" \"" << s.id() << "\"\n";
	os << std::setw(s.depth()) << "" << "\t\"Plane\" \"" << s.plane().toStr() << "\"\n";
	os << std::setw(s.depth()) << "" << "\t\"uaxis\" \"" << s.uaxis.toStr() << "\"\n";
	os << std::setw(s.depth()) << "" << "\t\"vaxis\" \"" << s.vaxis.toStr() << "\"\n";
	for (const auto& pair : s.keyvals)
	{
		os << std::setw(s.depth()) << "" << "\t" << KeyValBase::toStr(pair) << "\n";
	}
	s.disp.depth(s.depth() + TABDEPTH);
	os << s.disp << "\n";
	os << std::setw(s.depth()) << "" << "}";
	return os;
}

