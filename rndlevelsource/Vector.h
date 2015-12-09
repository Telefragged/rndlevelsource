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
	void rotate(const Matrix&);

	Vector& operator+=(const Vector&);
	Vector& operator-=(const Vector&);

	Vector& operator*=(double);

	static Vector allMinMax(const Vertex&, const Vertex&);

	std::string toStr() const;

	Vector();
	Vector(const Vertex&);
	Vector(double, double, double);
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

