#pragma once

#include "Vertex.h"
#include "Polygon.h"

class Vector;

class BoundingBox
{
public:
	Vertex min;
	Vertex max;

	//Returns true if lhs collides with rhs. Returns false otherwise.
	static bool testCollision(const BoundingBox& lhs, const BoundingBox& rhs);
	
	bool testCollision(const Vector& line, int flags = Polygon::lineBoundsFlag::ALLOW_NONE);

	std::string toStr() const;

	BoundingBox() = default;
	BoundingBox(const Vector& v);
	BoundingBox(const Vertex& min, const Vertex& max);
	~BoundingBox() = default;
};

