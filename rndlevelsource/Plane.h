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

	char evaluate(const Vertex&) const;

	Vertex closestAxisToNormal() const;

	static Plane vectorPlane(const Vector&, const Vector&);
	static Plane flip(const Plane&);

	static Vector intersectLine(const Plane&, const Plane&);
	static Vertex intersectPoint(const Plane&, const Vector&);

	static double dist(const Plane&, const Vertex&);
	static double dist(const Vector&, const Vertex&);
	static double dist(const Vertex&, const Vertex&);

	void parsestr(std::string);

	Plane(void);
	Plane(const std::string &);
	Plane(const Vertex &, const Vertex &, const Vertex &);
	~Plane(void);
};

