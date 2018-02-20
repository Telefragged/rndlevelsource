#include "Solid.h"

#include <algorithm>
#include <numeric>
#include <map>

#include "BoundingBox.h"
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
	copy.polygon.flip();
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

void Solid::scale(const Vertex& scale)
{
	for (auto &side : sides)
		side.polygon.scale(scale);
}

void Solid::reID(unsigned int &solidID, unsigned int &sideID)
{
	id_ = ++solidID;
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
		side.polygon.roundPoints();
	}
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
	Vertex point{ 0, 0, 0 };
	size_t numPoints = 0;

	for (const auto &side : sides)
	{
		point = std::accumulate(side.polygon.points.cbegin(), side.polygon.points.cend(), point);
		numPoints += side.polygon.points.size();
	}

	return point / double(numPoints);
}

Solid::Solid() :
	id_(0)
{
}

Solid::Solid(const std::initializer_list<Plane>& planes) : Solid()
{
	for (const auto &plane : planes)
	{
		Side side;
		side.polygon = { plane };
		for (const auto &other : planes)
			if (&plane != &other)
				side.polygon.slice(other);

		sides.push_back(side);
	}
}

Solid::~Solid()
{
}

