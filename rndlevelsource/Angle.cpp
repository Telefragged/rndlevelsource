#include "stdafx.h"
#include <math.h>
#include <assert.h>
#include <sstream>
#include "Angle.h"
#include "Matrix.h"
#include "Quaternion.h"

#define M_PI 3.14159265358979323846

#define DEG2RAD(X) (X*(M_PI/180.0))
#define RAD2DEG(X) (X*(180.0/M_PI))

//void inline sincos( double radians, float *sine, float *cosine ) {
//	#if defined( _WIN32 )
//	_asm
//	{
//		fld		DWORD PTR [radians]
//		fsincos
//
//		mov edx, DWORD PTR [cosine]
//		mov eax, DWORD PTR [sine]
//
//		fstp DWORD PTR [edx]
//		fstp DWORD PTR [eax]
//	}
//#elif defined( _LINUX )
//	register double __cosr, __sinr;
// 	__asm __volatile__
//    		("fsincos"
//     	: "=t" (__cosr), "=u" (__sinr) : "0" (radians));
//
//  	*sine = __sinr;
//  	*cosine = __cosr;
//#endif
//}

Angle::Angle(void) : Vertex()
{
}

Angle::Angle(const std::string& str) : Vertex(str)
{
}

double Angle::pitch() const
{
	return x();
}

double Angle::yaw() const
{
	return y();
}

double Angle::roll() const
{
	return z();
}

Vertex Angle::toVertex() const
{
	Vertex ret;
	double yawrad = vertex_[YAW] * (M_PI / 180.0), rollrad = vertex_[ROLL] * (M_PI / 180.0);
	ret.x(cos(yawrad) * cos(rollrad));
	ret.y(sin(yawrad) * cos(rollrad));
	ret.z(sin(rollrad));
	return ret;
}

Matrix Angle::angleMatrix() const
{
	double sinpitch = sin(DEG2RAD((*this)[PITCH])),
		cospitch = cos(DEG2RAD((*this)[PITCH])),
		sinyaw = sin(DEG2RAD((*this)[YAW])),
		cosyaw = cos(DEG2RAD((*this)[YAW])),
		sinroll = sin(DEG2RAD((*this)[ROLL])),
		cosroll = cos(DEG2RAD((*this)[ROLL]));

	Matrix matrix(3, 3);

	matrix[0][0] = cospitch * cosyaw;
	matrix[1][0] = cospitch * sinyaw;
	matrix[2][0] = -sinpitch;

	double crcy = cosroll * cosyaw;
	double crsy = cosroll * sinyaw;
	double srcy = sinroll * cosyaw;
	double srsy = sinroll * sinyaw;

	matrix[0][1] = sinpitch * srcy - crsy;
	matrix[1][1] = sinpitch * srsy + crcy;
	matrix[2][1] = sinroll * cospitch;

	matrix[0][2] = (sinpitch * crcy + srsy);
	matrix[1][2] = (sinpitch * crsy - srcy);
	matrix[2][2] = cosroll * cospitch;

	return matrix;
}

Matrix Angle::calcRotation(const Angle& target) const
{
	Quaternion curQuat(*this);
	Quaternion targetQuat(target);
	curQuat.normalize();
	targetQuat.normalize();
	Quaternion rot = targetQuat * curQuat.conjugate();
	rot.normalize();
	return rot.quaternionMatrix();
}

void Angle::fromMatrix(const Matrix& matrix)
{
	assert(matrix.x() == 3 && matrix.y() == 3);
	double forward[3];
	double left[3];
	double up[3];


	forward[0] = matrix[0][0];
	forward[1] = matrix[1][0];
	forward[2] = matrix[2][0];
	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];
	up[2] = matrix[2][2];

	double xyDist = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);

	if (xyDist > 0.0001)
	{
		(*this)[PITCH] = RAD2DEG( atan2( -forward[2], xyDist ) );
		(*this)[YAW] = RAD2DEG( atan2( forward[1], forward[0] ) );
		(*this)[ROLL] = RAD2DEG( atan2( left[2], up[2] ) );
	}
	else
	{ // gimbal lock
		(*this)[PITCH] = RAD2DEG( atan2( -forward[2], xyDist ) );
		(*this)[YAW] = RAD2DEG( atan2( -left[0], left[1] ) );
		(*this)[ROLL] = 0;
	}
}

std::string Angle::toStr() const
{
	std::ostringstream os;
	os << (*this)[PITCH] << " "
		<< (*this)[YAW] << " "
		<< (*this)[ROLL];
	return os.str();
}

bool Angle::isAngle(const Angle& a)
{
	return (a.x() == a.x() && a.y() == a.y() && a.z() == a.z());
}

Angle& Angle::operator+=(const Angle& rhs)
{
	this->vertex_[0] += rhs.vertex_[0];
	this->vertex_[1] += rhs.vertex_[1];
	this->vertex_[2] += rhs.vertex_[2];
	return *this;
}

Angle& Angle::operator-=(const Angle& rhs)
{
	this->vertex_[0] -= rhs.vertex_[0];
	this->vertex_[1] -= rhs.vertex_[1];
	this->vertex_[2] -= rhs.vertex_[2];
	return *this;
}

Angle Angle::operator-() const
{
	Angle ret;
	if (x() > 0) ret.x(x() - 180.0);
	else ret.x(x() + 180.0);
	if (y() > 0) ret.y(y() - 180.0);
	else ret.y(y() + 180.0);
	if (z() > 0) ret.z(z() - 180.0);
	else ret.z(z() + 180.0);
	return ret;
}

Angle::Angle(double x, double y, double z)
{
	vertex_[0] = x;
	vertex_[1] = y;
	vertex_[2] = z;
}

Angle::~Angle(void)
{
}

