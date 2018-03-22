#pragma once
#include <vector>

#include "Plane.h"

class Vector;
class Vertex;

class Polygon
{
public:
	enum classification
	{
		onPlane,
		front,
		back,
		spanning
	};

	enum lineBoundsFlag
	{
		ALLOW_NONE = 0x0,
		ALLOW_BACK = 0x1,
		ALLOW_FRONT = 0x2,
		ALLOW_BOTH = ALLOW_BACK | ALLOW_FRONT,
		RETURN_END_ON_FAIL = 0x4
	};

	std::vector<Vertex> points;

	classification classify(const Plane& plane) const;

	Vertex origin() const;
	Plane plane() const;

	void rotate(const Vertex& point, const Matrix3d& rotmat);
	void move(const Vertex& v);
	void moveTo(const Vertex& p);

	void scale(const Vertex& scale);
	void scale(const Vertex& origin, const Vertex& scale);

	void sliceThis(const Plane& plane);
	std::pair<Polygon, Polygon> slice(const Plane& plane) const;

	void flip();
	void roundPoints(size_t precision = 3);

	Vertex intersectPoint(const Vector& line, int flags = lineBoundsFlag::ALLOW_BOTH) const;

	bool testCollision(const Vertex& point) const;
	bool testCollision(const Vector& line, int flags = lineBoundsFlag::ALLOW_BOTH) const;

	Polygon() = default;
	Polygon(const Plane& p);
	Polygon(const std::initializer_list<Vertex>& points);
	~Polygon() = default;
};
