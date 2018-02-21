#pragma once
#include "Vertex.h"
#include "Matrix.h"

class Vector;

class Plane
{
public:
	Vertex p1, p2, p3;
	std::string toStr() const;

	Vertex normal() const;

	//Equation for this plane in form x + y + z + d = 0,
	//returned as a 1x4 matrix.
	Matrix<double, 1, 4> equation() const;

	char evaluate(const Vertex& point) const;

	Vertex closestAxisToNormal() const;

	static Plane vectorPlane(const Vector& line1, const Vector& line2);
	static Plane flip(const Plane& plane);

	static Vector intersectLine(const Plane& lhs, const Plane& rhs);
	static Vertex intersectPoint(const Plane& p, const Vector& line);

	static double dist(const Plane& plane, const Vertex& point);
	static double dist(const Vector& line, const Vertex& point);
	static double dist(const Vertex& point, const Vertex& other);

	void parsestr(const std::string& pstr);

	Plane() = default;
	Plane(const std::string & str);
	Plane(const Vertex& p1, const Vertex& p2, const Vertex& p3);
	~Plane() = default;
};

