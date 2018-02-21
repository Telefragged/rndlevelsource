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

	classification classify(const Plane& plane) const;

	Vertex origin() const;

	void rotate(const Vertex& point, const Matrix3d& rotmat);
	void move(const Vertex& v);
	void moveTo(const Vertex& p);

	void scale(const Vertex& scale);
	void scale(const Vertex& origin, const Vertex& scale);

	void slice(const Plane& plane);
	bool slice(const Plane& plane, Polygon& back, Polygon& front) const;

	void flip();
	void roundPoints(size_t precision = 3);

	Polygon() = default;
	Polygon(const Plane& p);
	Polygon(const std::initializer_list<Vertex>& points);
	~Polygon() = default;
};
