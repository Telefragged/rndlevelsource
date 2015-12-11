#pragma
#include <ios>
#include <vector>
#include "Editor.h"
#include "KeyValBase.h"
#include "Side.h"

class BoundingBox;
class Matrix;
class Vector;
class Vertex;

class Solid :
	public KeyValBase
{
private:
	unsigned int id_;
	mutable unsigned int depth_;
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

	static Solid createBox(const Vector&, std::string texture = "tools/toolsnodraw", int texturemode = 0, std::string othertexture = "tools/toolsnodraw");

	static std::vector<Solid> carveBox(const Vector& size, const Solid& initial);

	static std::vector<Solid> slice(const Solid &solid, const Plane &plane);

	unsigned int parse(std::istream&);
	void rotate(const Vertex&, const Matrix&);
	void move(const Vector&);
	void reID(unsigned int*, unsigned int*);

	unsigned int depth() const;

	void depth(unsigned int newDepth) const;

	BoundingBox bbox() const;

	friend std::ostream& operator<<(std::ostream&, const Solid&);

	Solid(void);
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

