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

Vertex Solid::intersectPoint(const Vector& line, int flags)
{
	Vertex ret = (flags & Polygon::lineBoundsFlag::RETURN_END_ON_FAIL) > 0 ? line.end() : Vertex();

	double minPos = std::numeric_limits<double>::infinity();

	for (auto &side : sides)
	{
		Vertex intersect = side.polygon.intersectPoint(line, flags & Polygon::lineBoundsFlag::ALLOW_BOTH);

		if (Vertex::isVertex(intersect))
		{
			double position = abs(line.calculatePosition(intersect));
			if (position < minPos)
			{
				minPos = position;
				ret = intersect;
			}
		}
	}

	return ret;
}

bool Solid::slice(const Plane& plane, Solid& front, Solid& back) const
{
	front = Solid();
	back = Solid();

	if (sides.empty())
		return false;

	std::vector<Polygon::classification> classes;
	classes.resize(sides.size());
	std::transform(sides.cbegin(), sides.cend(), classes.begin(),
		[&plane](const Side &side) {return side.polygon.classify(plane); });

	if (std::count_if(classes.cbegin(), classes.cend(),
		[](auto &c) {return c == Polygon::classification::spanning; }) == 0)
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

size_t Solid::parseSpecial(std::istream & stream, std::string_view type)
{
	if (type == "side")
	{
		Side side;
		auto ret = side.parse(stream);
		side.popuvars();
		addSide(side);
		return ret;
	}

	if (type == "editor")
		return edt.parse(stream);

	return 0;
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
		s.move(vec);
}

void Solid::moveTo(const Vertex& point)
{
	Vector diff = Vector::diff(origin(), point);
	move(diff);
}

void Solid::scale(const Vertex& scale)
{
	Vertex origin = this->origin();
	this->scale(scale, origin);
}

void Solid::scale(const Vertex & scale, const Vertex & origin)
{
	for (auto &side : sides)
		side.polygon.scale(origin, scale);
}

void Solid::reID(unsigned int& solidID, unsigned int& sideID)
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
		other.polygon.sliceThis(tmp.plane());
		tmp.polygon.sliceThis(other.plane());
	}

	sides.push_back(std::move(tmp));
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
	constexpr double dmax = std::numeric_limits<double>::max();
	constexpr double dmin = -dmax;
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

void Solid::extraOutput(std::ostream& os) const
{
	for (const Side& sd : sides)
	{
		sd.depth(depth() + TABDEPTH);
		os << sd << "\n";
	}
	edt.depth(depth() + TABDEPTH);
	os << edt << "\n";
}

std::string Solid::getName() const
{
	return "solid";
}

bool Solid::empty() const
{
	return keyvals.empty() && sides.empty();
}

Solid::Solid(const BoundingBox & b)
{
	Plane under = { { b.min.x(), b.min.y(), b.min.z() },{ b.max.x(), b.min.y(), b.min.z() },{ b.max.x(), b.max.y(), b.min.z() } };
	Plane over = { { b.max.x(), b.max.y(), b.max.z() },{ b.max.x(), b.min.y(), b.max.z() },{ b.min.x(), b.min.y(), b.max.z() } };
	Plane front = { { b.min.x(), b.min.y(), b.min.z() },{ b.min.x(), b.min.y(), b.max.z() },{ b.max.x(), b.min.y(), b.max.z() } };
	Plane back = { { b.min.x(), b.max.y(), b.min.z() },{ b.max.x(), b.max.y(), b.min.z() },{ b.max.x(), b.max.y(), b.max.z() } };
	Plane left = { { b.min.x(), b.max.y(), b.min.z() },{ b.min.x(), b.max.y(), b.max.z() },{ b.min.x(), b.min.y(), b.max.z() } };
	Plane right = { { b.max.x(), b.min.y(), b.min.z() },{ b.max.x(), b.min.y(), b.max.z() },{ b.max.x(), b.max.y(), b.max.z() } };

	*this = Solid{ under, over, front, back, left, right };
}

Solid::Solid(const std::initializer_list<Plane>& planes) : Solid()
{
	for (const auto &plane : planes)
	{
		Side side;
		side.polygon = { plane };
		addSide(side);
	}
}
