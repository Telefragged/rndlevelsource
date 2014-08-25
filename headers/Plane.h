#pragma once
#include "Vertex.h"

class Matrix;
class Vector;

class Plane
{
public:
	Vertex p1, p2, p3;
	std::string toStr() const;

	Vertex normal() const;

	//Equation for this plane in form x + y + z + d = 0,
	//returned as a 1x4 matrix.
	Matrix equation() const;

	//Returns a vector representing the line where two planes intersect.
	//Parameters are equations in the form ax + by + cz + d = 0,
	//as a 1x4 matrix.
	static Plane vectorPlane(const Vector &, const Vector &);

	static Vector intersectLine(const Matrix &, const Matrix &);

	static double dist(const Vertex &, const Vector &);
	static double dist(const Vertex &, const Vertex &);

	static bool crossesLine(const Plane &, const Vector &);
	static bool testCollision(const Plane &, const Plane &);

	void parsestr(std::string);

	Plane(void);
	~Plane(void);
};
