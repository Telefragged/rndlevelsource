#pragma once
#include "Vertex.h"
#include "Matrix.h"

class Angle;

class Quaternion
{
	Vertex xyz_;
	double w_;
public:

	double x() const;
	double y() const;
	double z() const;
	double w() const;

	double magnitude() const;

	void normalize();
	void alignWith(const Quaternion&);

	Matrix3d quaternionMatrix() const;

	Quaternion conjugate();

	Quaternion& operator*=(const Quaternion& rhs);

	double& operator[](size_t pos);
	double operator[](size_t pos) const;

	Quaternion();
	Quaternion(double x, double y, double z, double w);
	Quaternion(const Angle& a);
	~Quaternion() = default;
};

inline Quaternion operator*(Quaternion lhs, const Quaternion& rhs)
{
	lhs *= rhs;
	return lhs;
}

