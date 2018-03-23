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

	std::vector<Side> sides;

	static bool testCollision(const Solid& lhs, const Solid& rhs);
	Vertex intersectPoint(const Vector& line, int flags = Polygon::lineBoundsFlag::ALLOW_BOTH);

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
	bool empty() const override;

	Solid() = default;
	Solid(const BoundingBox& b);
	Solid(const std::initializer_list<Plane>& planes);
	~Solid() override = default;
};
