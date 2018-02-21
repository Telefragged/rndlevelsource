#pragma once

#include "Vertex.h"

class Vector;

class BoundingBox
{
public:
	Vertex min;
	Vertex max;

	//Returns true if lhs collides with rhs. Returns false otherwise.
	static bool testCollision(const BoundingBox& lhs, const BoundingBox& rhs);

	std::string toStr() const;

	BoundingBox();
	BoundingBox(const Vector& v);
	BoundingBox(const Vertex& min, const Vertex& max);
	~BoundingBox() = default;
};

