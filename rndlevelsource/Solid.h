#pragma once
#include <vector>
#include "Editor.h"
#include "KeyValBase.h"
#include "Side.h"
#include "Polygon.h"
#include "Matrix.h"

class BoundingBox;
class Vector;
class Vertex;

class Solid :
	public KeyValBase
{
private:
	size_t id_ = 0;
	Editor edt;
public:

	enum texmode
	{
		UP = 0x0001,
		DOWN = 0x0002,
		NORTH = 0x0004,
		SOUTH = 0x0008,
		WEST = 0x0010,
		EAST = 0x0020,
		INSIDE = 0x0040
	};

	std::vector<Side> sides;

	static bool testCollision(const Solid& lhs, const Solid& rhs);

	bool slice(const Plane& plane, Solid& front, Solid& back) const;

	unsigned int parse(std::istream& stream);
	void rotate(const Vertex& point, const Matrix3d& rotmat);
	void move(const Vector& vec);
	void moveTo(const Vertex& point);

	void scale(const Vertex& scale);
	void scale(const Vertex& scale, const Vertex& origin);
	void reID(unsigned int& solidID, unsigned int& sideID);
	void addSide(const Side& side);
	void fixSides();

	BoundingBox bbox() const;

	Vertex origin() const;

	friend std::ostream& operator<<(std::ostream& os, const Solid& s);

	Solid();
	Solid(const std::initializer_list<Plane>& planes);
	~Solid();
};

inline std::ostream& operator<<(std::ostream& os, const Solid& s)
{
	os << std::setw(s.depth()) << "" << "solid\n";
	os << std::setw(s.depth()) << "" << "{\n";
	os << std::setw(s.depth()) << "" << "\t\"id\" \"" << s.id_ << "\"\n";
	for (const Side& sd : s.sides)
	{
		sd.depth(s.depth() + TABDEPTH);
		os << sd << "\n";
	}
	s.edt.depth(s.depth() + TABDEPTH);
	os << s.edt << "\n";
	os << std::setw(s.depth()) << "" << "}";
	return os;
}
