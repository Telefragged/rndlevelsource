#include "Side.h"

#include <ios>
#include <stdio.h>

#include "BoundingBox.h"
#include "KeyVal.h"
#include "Vector.h"

size_t Side::parseSpecial(std::istream & stream, std::string_view type)
{
	if (type == "dispinfo")
	{
		disp = std::make_shared<DispInfo>();
		return disp->parse(stream);
	}

	return 0;
}

void Side::reID(unsigned int& sideID)
{
	id_ = ++sideID;
}

void Side::popuvars()
{
	if (auto it = keyvals.find("id"); it != keyvals.end())
	{
		this->id_ = atoi(it->second.c_str());
		keyvals.erase(it);
	}
	if (auto it = keyvals.find("plane"); it != keyvals.end())
	{
		this->polygon = Polygon(Plane(it->second));
		keyvals.erase(it);
	}
	if (auto it = keyvals.find("uaxis"); it != keyvals.end())
	{
		this->uaxis.parsestr(it->second);
		keyvals.erase(it);
	}
	if (auto it = keyvals.find("vaxis"); it != keyvals.end())
	{
		this->vaxis.parsestr(it->second);
		keyvals.erase(it);
	}
}

void Side::rotate(const Vertex& point, const Matrix3d& rotmat)
{
	//mictimer rotatetimer("ms Side::rotate()", 1000.0);
	polygon.rotate(point, rotmat);
	uaxis.v = uaxis.v.rotate(rotmat);
	vaxis.v = vaxis.v.rotate(rotmat);

	if(disp != nullptr)
		disp->rotate(point, rotmat);
}

void Side::move(const Vector& vec)
{
	Vertex mov = vec.vec();
	polygon.move(mov);
	uaxis.translate(mov);
	vaxis.translate(mov);
}

Plane Side::plane() const
{
	if (polygon.points.size() <= 2)
		throw std::exception("Not enough data to construct plane from polygon");
	return Plane(polygon.points[0], polygon.points[1], polygon.points[2]);
}

BoundingBox Side::bbox() const
{
	Vertex min, max;

	min = max = polygon.points[0];

	for (size_t i = 1; i < polygon.points.size(); i++)
	{
		min = Vertex::allmin(min, polygon.points[i]);
		max = Vertex::allmax(max, polygon.points[i]);
	}

	return BoundingBox(min, max);
}

bool Side::testCollision(const Side& lhs, const Side& rhs)
{
	return false;
}

void Side::extraOutput(std::ostream& os) const
{
	os << std::setw(depth()) << "" << "\t\"plane\" \"" << plane() << "\"\n";
	os << std::setw(depth()) << "" << "\t\"uaxis\" \"" << uaxis << "\"\n";
	os << std::setw(depth()) << "" << "\t\"vaxis\" \"" << vaxis << "\"\n";
	if (disp != nullptr)
	{
		disp->depth(depth() + TABDEPTH);
		os << disp << "\n";
	}
}

std::string Side::getName() const
{
	return "side";
}
