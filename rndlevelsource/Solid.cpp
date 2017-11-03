#include <algorithm>
#include <numeric>
#include <map>
#include "stdafx.h"
#include "BoundingBox.h"
#include "Solid.h"
#include "Vector.h"
#include "Matrix.h"

bool Solid::testCollision(const Solid& lhs, const Solid& rhs)
{
	if (BoundingBox::testCollision(lhs.bbox(), rhs.bbox()))
	{
		for (const Side& lhss : lhs.sides)
		{
			for (const Side& rhss : rhs.sides)
			{
				if (Side::testCollision(lhss, rhss)) return true;
			}
		}
	}
	return false;
}

bool Solid::slice(const Plane& plane, Solid& front, Solid& back) const
{
	front = Solid();
	back = Solid();

	if (sides.size() == 0)
		return false;

	std::vector<Polygon::classification> classes;
	classes.resize(sides.size());
	std::transform(sides.cbegin(), sides.cend(), classes.begin(),
		[&plane](const Side &side) {return side.polygon.classify(plane); });

	if (std::count_if(classes.cbegin(), classes.cend(),
		[](auto &c) {return c != Polygon::classification::spanning; }) == classes.size())
	{
		if (std::count(classes.cbegin(), classes.cend(), Polygon::classification::back) > 0)
			back = *this;
		else if (std::count(classes.cbegin(), classes.cend(), Polygon::classification::front) > 0)
			front = *this;
		return false;
	}

	Side copy = sides[0];
	copy.polygon = { plane };
	back.addSide(copy);
	copy.polygon = { Plane::flip(plane) };
	front.addSide(copy);

	for(auto &side : sides)
	{
		copy = side;
		copy.polygon = { side.plane() };
		Polygon::classification classification = side.polygon.classify(plane);
		if (classification != Polygon::classification::back) front.addSide(copy);
		if (classification != Polygon::classification::front) back.addSide(copy);
	}

	return true;
}

unsigned int Solid::parse(std::istream& stream)
{
	unsigned int numparsed = 0;
	std::string curline;
	while (trim(curline) != "{")
	{
		getline(stream, curline);
		numparsed++;
	}
	unsigned int depth = 1;
	while (getline(stream, curline))
	{
		numparsed++;
		if (trim(curline) == "side")
		{
			Side side;
			numparsed += side.parse(stream);
			addSide(side);
		}
		else if (trim(curline) == "editor")
		{
			numparsed += edt.parse(stream);
		}
		else if (trim(curline) == "}")
		{
			if (--depth == 0) break;
		}
		else
		{
			KeyVal k(curline);
			if (k.key == "id") id_ = k.toInt();
		}
	}

	fixSides();

	return numparsed;
}

void Solid::rotate(const Vertex& point, const Matrix3d& rotmat)
{
	//mictimer rottimer("ms Solid::rotate()", 1000.0);
	for (Side& s : sides)
	{
		s.rotate(point, rotmat);
	}
}

void Solid::move(const Vector& vec)
{
	for (Side& s : sides)
	{
		s.move(vec);
	}
}

void Solid::reID(unsigned int* solidID, unsigned int* sideID)
{
	id_ = ++(*solidID);
	for (Side& side : sides)
	{
		side.reID(sideID);
	}
}

void Solid::addSide(const Side &side)
{
	Side tmp = side;

	for (auto &other : sides)
	{
		other.polygon.slice(tmp.plane());
		tmp.polygon.slice(other.plane());
	}

	sides.push_back(tmp);
}

void Solid::fixSides()
{
	auto orig = origin();
	for (auto &side : sides)
	{
		if (side.plane().evaluate(orig) >= 0)
		{
			side.polygon.flip();
		}
		side.polygon.roundPoints(3);
	}
}

unsigned Solid::depth() const
{
	return depth_;
}

void Solid::depth(unsigned int newDepth) const
{
	depth_ = newDepth;
}

BoundingBox Solid::bbox() const
{
	const double dmax = std::numeric_limits<double>::max();
	const double dmin = -dmax;
	Vertex min(dmax, dmax, dmax);
	Vertex max(dmin, dmin, dmin);
	for (const Side& s : sides)
	{
		BoundingBox sbox = s.bbox();
		min = Vertex::allmin(min, sbox.min);
		max = Vertex::allmax(max, sbox.max);
	}
	return BoundingBox(min, max);
}

Vertex Solid::origin() const
{
	std::vector<Vertex> points;

	for (const auto &side : sides)
		points.insert(points.end(), side.polygon.points.cbegin(), side.polygon.points.cend());

	return std::accumulate(points.begin(), points.end(), Vertex{ 0, 0, 0 }) / points.size();

	//BoundingBox b = bbox();
	//Vector vec = Vector::diff(b.min, b.max);
	//Vertex mod = 0.5 * vec.vec();
	//return vec.beg() + mod;
}

Solid::Solid(void) :
	id_(0),
	depth_(0)
{
}

Solid::Solid(const std::vector<Plane>& planes) : Solid()
{
	for (size_t n = 0; n < planes.size(); n++)
	{
		Side side;
		side.polygon = { planes[n] };
		for (size_t m = 0; m < planes.size(); m++)
			if (n != m)
				side.polygon.slice(planes[m]);

		sides.push_back(side);
	}
}

Solid::~Solid(void)
{
}

