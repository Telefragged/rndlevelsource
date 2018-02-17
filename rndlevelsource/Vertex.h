#pragma once

#include <string>
#include "Matrix.h"

// For all intents and purposes a vector from the
// coordinate systems origin.

class Vertex
{
protected:
	double vertex_[3];
public:

	static const Vertex unitX, unitY, unitZ;

	virtual std::string toStr() const;

	static bool isVertex(const Vertex&);

	static Vertex allmin(const Vertex&, const Vertex&);
	static Vertex allmax(const Vertex&, const Vertex&);

	void parsestr(const std::string&);

	Vertex rotate(const Matrix3d&) const;
	Vertex rotate(const Vertex&, const Matrix3d&) const;

	void rotateInPlace(const Matrix3d&);
	void rotateInPlace(const Vertex&, const Matrix3d&);

	virtual void x(double x);
	virtual void y(double y);
	virtual void z(double z);

	virtual double x() const;
	virtual double y() const;
	virtual double z() const;

	Matrix<double, 3, 1> toMat() const;

	double length() const;
	double dotProduct(const Vertex&) const;

	Vertex closestAxis() const;
	Vertex normalize() const;
	Vertex crossProduct(const Vertex&) const;

	static bool parallel(const Vertex&, const Vertex&);
	static bool equals(const Vertex&, const Vertex&);

	static double length(const Vertex&);
	static double dotProduct(const Vertex&, const Vertex&);
	static double dotProduct2D(const Vertex&, const Vertex&);

	static Vertex absolute(const Vertex&);
	static Vertex normalize(const Vertex&);
	static Vertex crossProduct(const Vertex&, const Vertex&);

	Vertex& operator=(const Vertex&);
	Vertex& operator+=(const Vertex&);
	Vertex& operator-=(const Vertex&);
	Vertex& operator*=(double);
	Vertex& operator/=(double);

	bool operator==(const Vertex&) const;
	bool operator<(const Vertex&) const;

	double operator[](unsigned int) const;
	double& operator[](unsigned int);

	Vertex();
	Vertex(const std::string&);
	Vertex(double, double, double);
	virtual ~Vertex();
};

inline Vertex operator+(Vertex lhs, const Vertex& rhs)
{
	lhs += rhs;
	return lhs;
}

inline Vertex operator-(Vertex lhs, const Vertex& rhs)
{
	lhs -= rhs;
	return lhs;
}

inline Vertex operator*(double lhs, Vertex rhs)
{
	rhs *= lhs;
	return rhs;
}

inline Vertex operator*(Vertex lhs, double rhs)
{
	lhs *= rhs;
	return lhs;
}

inline Vertex operator/(double lhs, Vertex rhs)
{
	rhs /= lhs;
	return rhs;
}

inline Vertex operator/(Vertex lhs, double rhs)
{
	lhs /= rhs;
	return lhs;
}

inline Vertex operator-(const Vertex& v)
{
	return -1 * v;
}
