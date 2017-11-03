#pragma once
#include <vector>
#include "Vertex.h"
#include "Vector.h"
#include "Plane.h"

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

	std::vector<Vertex> points;

	classification classify(const Plane&) const;

	Vertex origin() const;

	void rotate(const Vertex& point, const Matrix3d& rotmat);
	void move(const Vertex& v);

	void slice(const Plane&);
	bool slice(const Plane&, Polygon&, Polygon&) const;

	Polygon();
	Polygon(const Plane& p);
	~Polygon();
};
