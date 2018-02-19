#pragma once
#include "Vertex.h"

//A vector with its own origin.
//For normal vectors, use Vertex class.
//TODO -- rename?
class Vector
{
private:
	Vertex _orig;
	Vertex _vec;
public:
	//The origin of the vector in XYZ space
	Vertex beg() const;
	void beg(const Vertex&);
	//The end point in XYZ space.
	Vertex end() const;
	//The internal vector.
	Vertex vec() const;
	void vec(const Vertex&);
	//Rotate the internal vector.
	//The origin is not changed.
	void rotate(const Matrix3d& rotmat);

	Vector& operator+=(const Vector&);
	Vector& operator-=(const Vector&);

	Vector& operator*=(double);

	//Returns a vector that points from vertex 1 to vertex 2
	static Vector diff(const Vertex&, const Vertex&);
	static Vector allMinMax(const Vertex&, const Vertex&);

	//Calculates the point where line A intersects with B.
	//Returns invalid Vertex if the lines to not intersect.
	static Vertex intersectPoint(const Vector&, const Vector&);

	std::string toStr() const;

	Vector();
	Vector(const Vertex&);
	Vector(double, double, double);

	//Construct vector with point and vector part
	Vector(const Vertex&, const Vertex&);
	~Vector();
};

inline Vector operator+(Vector lhs, const Vector& rhs)
{
	lhs += rhs;
	return lhs;
}

inline Vector operator-(Vector lhs, const Vector& rhs)
{
	lhs -= rhs;
	return lhs;
}

inline Vector operator*(Vector lhs, double rhs)
{
	lhs *= rhs;
	return lhs;
}

