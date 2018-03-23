#pragma once
#include "Vertex.h"

//A vector with its own origin.
//For normal vectors, use Vertex class.
//TODO -- rename?
class Vector
{
private:
	Vertex orig_;
	Vertex vec_;
public:
	//The origin of the vector in XYZ space
	Vertex beg() const;
	void beg(const Vertex& beg);
	//The end point in XYZ space.
	Vertex end() const;
	//The internal vector.
	Vertex vec() const;
	void vec(const Vertex& vec);

	//Rotate the internal vector.
	//The origin is not changed.
	void rotate(const Matrix3d& rotmat);

	//Set internal vector length
	void setLength(double length);

	Vector& operator+=(const Vector& rhs);
	Vector& operator-=(const Vector& rhs);

	Vector& operator*=(double rhs);

	//Returns a vector that points from vertex 1 to vertex 2
	static Vector diff(const Vertex& p1, const Vertex& p2);
	static Vector allMinMax(const Vertex& v1, const Vertex& v2);

	//Calculates the point where line A intersects with B.
	//Returns invalid Vertex if the lines to not intersect.
	static Vertex intersectPoint(const Vector& lhs, const Vector& rhs);

	bool liesOnLine(const Vertex& v) const;

	double calculatePosition(const Vertex& v) const;

	std::string toStr() const;

	Vector() = default;
	Vector(const Vertex& vert);
	Vector(double x, double y, double z);

	//Construct vector with point and vector part
	Vector(const Vertex& beg, const Vertex& vec);
	~Vector() = default;
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

