#pragma once
#include "Vertex.h"

class Matrix;
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

	Matrix quaternionMatrix();

	Quaternion conjugate();

	Quaternion& operator*=(const Quaternion&);

	double& operator[](unsigned int);
	double operator[](unsigned int) const;

	Quaternion(void);
	Quaternion(double, double, double, double);
	Quaternion(const Angle&);
	~Quaternion(void);
};

inline Quaternion operator*(Quaternion lhs, const Quaternion& rhs)
{
	lhs *= rhs;
	return lhs;
}

