#pragma
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
	size_t id_;
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

	static bool testCollision(const Solid&, const Solid&);

	bool slice(const Plane& plane, Solid& front, Solid& back) const;

	unsigned int parse(std::istream&);
	void rotate(const Vertex&, const Matrix3d&);
	void move(const Vector&);
	void reID(unsigned int&, unsigned int&);
	void addSide(const Side& side);
	void fixSides();

	BoundingBox bbox() const;

	Vertex origin() const;

	friend std::ostream& operator<<(std::ostream&, const Solid&);

	Solid(void);
	Solid(const std::vector<Plane>& planes);
	~Solid(void);
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
