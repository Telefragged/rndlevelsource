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
	//The end point in XYZ space.
	Vertex end() const;
	//The internal vector.
	Vertex vec() const;
	void vec(const Vertex &);
	//Rotate the internal vector.
	//The origin is not changed.
	void rotate(const Matrix &);

	Vector &operator+=(const Vector &);
	Vector &operator-=(const Vector &);

	Vector(void);
	Vector(const Vertex &);
	Vector(const Vertex &, const Vertex &);
	~Vector(void);
};

inline Vector operator+(Vector lhs, const Vector &rhs) {
	lhs+=rhs;
	return lhs;
}

inline Vector operator-(Vector lhs, const Vector &rhs) {
	lhs-=rhs;
	return lhs;
}
