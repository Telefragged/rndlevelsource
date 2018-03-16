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
public:
	Editor edt;

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

	size_t parseSpecial(std::istream& stream, std::string_view type) override;

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

	void extraOutput(std::ostream& os) const override;
	std::string getName() const override;

	Solid() = default;
	Solid(const std::initializer_list<Plane>& planes);
	~Solid() override = default;
};
