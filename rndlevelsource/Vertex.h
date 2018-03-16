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

	static bool isVertex(const Vertex& v);

	static Vertex allmin(const Vertex& v1, const Vertex& v2);
	static Vertex allmax(const Vertex& v1, const Vertex& v2);

	void parsestr(std::string_view str);

	Vertex rotate(const Matrix3d& rotmat) const;
	Vertex rotate(const Vertex& point, const Matrix3d& rotmat) const;

	void rotateInPlace(const Matrix3d& rotmat);
	void rotateInPlace(const Vertex& point, const Matrix3d& rotmat);

	virtual void x(double x);
	virtual void y(double y);
	virtual void z(double z);

	virtual double x() const;
	virtual double y() const;
	virtual double z() const;

	Matrix<double, 3, 1> toMat() const;

	double length() const;
	double dotProduct(const Vertex& rhs) const;

	Vertex closestAxis() const;
	Vertex normalize() const;
	Vertex crossProduct(const Vertex& rhs) const;

	static bool parallel(const Vertex& lhs, const Vertex& rhs);
	static bool equals(const Vertex& lhs, const Vertex& rhs);

	static double length(const Vertex& v);
	static double dotProduct(const Vertex& lhs, const Vertex& rhs);
	static double dotProduct2D(const Vertex& lhs, const Vertex& rhs);

	static Vertex absolute(const Vertex& v);
	static Vertex normalize(const Vertex& v);
	static Vertex crossProduct(const Vertex& lhs, const Vertex& rhs);

	static size_t countDifferentAxes(const Vertex& lhs, const Vertex& rhs);

	Vertex& operator=(const Vertex& rhs);
	Vertex& operator+=(const Vertex& rhs);
	Vertex& operator-=(const Vertex& rhs);
	Vertex& operator*=(double mod);
	Vertex& operator/=(double mod);

	bool operator==(const Vertex& rhs) const;
	bool operator<(const Vertex& rhs) const;

	double operator[](size_t pos) const;
	double& operator[](size_t pos);

	Vertex();
	Vertex(const std::string& str);
	Vertex(double x, double y, double z);
	virtual ~Vertex();
};

inline std::ostream& operator<<(std::ostream& os, const Vertex& v)
{
	os << v.x() << ' ' << v.y() << ' ' << v.z();
	return os;
}

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
