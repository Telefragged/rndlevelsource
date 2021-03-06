#include "Quaternion.h"

#include <math.h>

#include "Angle.h"
#include "Matrix.h"

#define M_PI 3.14159265358979323846

#define DEG2RAD(_x) _x*(M_PI/180.0)
#define RAD2DEG(_x) _x*(180.0/M_PI)

double Quaternion::x() const
{
	return xyz_.x();
}

double Quaternion::y() const
{
	return xyz_.y();
}

double Quaternion::z() const
{
	return xyz_.z();
}

double Quaternion::w() const
{
	return w_;
}

//pop pop!!
double Quaternion::magnitude() const
{
	return sqrt(
		(x() * x()) +
		(y() * y()) +
		(z() * z()) +
		(w() * w()));
}

void Quaternion::normalize()
{
	double mag = magnitude();
	xyz_.x(xyz_.x() / mag);
	xyz_.y(xyz_.y() / mag);
	xyz_.z(xyz_.z() / mag);
	w_ = w_ / mag;
}

Matrix3d Quaternion::quaternionMatrix() const
{
	//Quaternion has to be a unit quaternion
	//for this to produce a correct result.
	Matrix3d matrix;

	matrix[0][0] = 1.0 - 2.0 * y() * y() - 2.0 * z() * z();
	matrix[1][0] = 2.0 * x() * y() + 2.0 * w() * z();
	matrix[2][0] = 2.0 * x() * z() - 2.0 * w() * y();

	matrix[0][1] = 2.0f * x() * y() - 2.0f * w() * z();
	matrix[1][1] = 1.0f - 2.0f * x() * x() - 2.0f * z() * z();
	matrix[2][1] = 2.0f * y() * z() + 2.0f * w() * x();

	matrix[0][2] = 2.0f * x() * z() + 2.0f * w() * y();
	matrix[1][2] = 2.0f * y() * z() - 2.0f * w() * x();
	matrix[2][2] = 1.0f - 2.0f * x() * x() - 2.0f * y() * y();

	return matrix;
}

Quaternion Quaternion::conjugate()
{
	return Quaternion(-x(), -y(), -z(), w());
}

Quaternion::Quaternion()
{
	w_ = std::numeric_limits<double>::quiet_NaN();
}

Quaternion& Quaternion::operator*=(const Quaternion& rhs)
{
	Quaternion lhs(*this); //temporary quaternion to preserve old values

	xyz_.x((lhs.w() * rhs.x()) + (lhs.x() * rhs.w()) + (lhs.y() * rhs.z()) - (lhs.z() * rhs.y()));
	xyz_.y((lhs.w() * rhs.y()) - (lhs.x() * rhs.z()) + (lhs.y() * rhs.w()) + (lhs.z() * rhs.x()));
	xyz_.z((lhs.w() * rhs.z()) + (lhs.x() * rhs.y()) - (lhs.y() * rhs.x()) + (lhs.z() * rhs.w()));
	w_ = (lhs.w() * rhs.w()) - (lhs.x() * rhs.x()) - (lhs.y() * rhs.y()) - (lhs.z() * rhs.z());

	return *this;
}

Angle Quaternion::toAngle()
{
	Angle ret;

	// roll (x-axis rotation)
	double sinr = 2.0 * (w() * x() + y() * z());
	double cosr = 1.0 - 2.0 * (x() * x() + y() * y());
	ret.roll(RAD2DEG(atan2(sinr, cosr)));

	// pitch (y-axis rotation)
	double sinp = 2.0 * (w() * y() - z() * x());
	if (fabs(sinp) >= 1)
		ret.pitch(RAD2DEG(copysign(M_PI / 2, sinp))); // use 90 degrees if out of range
	else
		ret.pitch(RAD2DEG(asin(sinp)));

	// yaw (z-axis rotation)
	double siny = 2.0 * (w() * z() + x() * y());
	double cosy = 1.0 - 2.0 * (y() * y() + z() * z());
	ret.yaw(RAD2DEG(atan2(siny, cosy)));

	return ret;
}

double Quaternion::operator[](size_t pos) const
{
	switch (pos)
	{
	case 1:
	case 2:
	case 3:
		return xyz_[pos];
	case 4:
		return w_;
	}
	return std::numeric_limits<double>::quiet_NaN();
}

double& Quaternion::operator[](size_t pos)
{
	switch (pos)
	{
	case 1:
	case 2:
	case 3:
		return xyz_[pos];
	default:
		return w_;
	}
}

Quaternion::Quaternion(double x, double y, double z, double w) :
	xyz_(x, y, z),
	w_(w)
{
}

Quaternion::Quaternion(const Angle& a)
{
	double sp = sin(DEG2RAD(a.yaw()) * 0.5);
	double cp = cos(DEG2RAD(a.yaw()) * 0.5);
	double sr = sin(DEG2RAD(a.pitch()) * 0.5);
	double cr = cos(DEG2RAD(a.pitch()) * 0.5);
	double sy = sin(DEG2RAD(a.roll()) * 0.5);
	double cy = cos(DEG2RAD(a.roll()) * 0.5);

	double srXcp = sy * cr;
	double crXsp = cy * sr;
	xyz_.x(srXcp * cp - crXsp * sp); // X
	xyz_.y(crXsp * cp + srXcp * sp); // Y

	double crXcp = cy * cr;
	double srXsp = sy * sr;
	xyz_.z(crXcp * sp - srXsp * cp); // Z
	w_ = crXcp * cp + srXsp * sp; // W (real component)
}
