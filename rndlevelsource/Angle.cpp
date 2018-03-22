#include "Angle.h"

#include <cmath>
#include <cassert>
#include <sstream>

#include "Matrix.h"
#include "Quaternion.h"
#include "Vertex.h"

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


Angle::Angle(std::string_view str)
{
	Vertex v(str);
	pitch_ = v.x();
	yaw_ = v.y();
	roll_ = v.z();
}

Angle::Angle(const Matrix3d & mat)
{
	this->fromMatrix(mat);
}

double Angle::pitch() const
{
	return pitch_;
}

void Angle::pitch(double pitch)
{
	this->pitch_ = pitch;
}

double Angle::yaw() const
{
	return yaw_;
}

void Angle::yaw(double yaw)
{
	this->yaw_ = yaw;
}

double Angle::roll() const
{
	return roll_;
}

void Angle::roll(double roll)
{
	this->roll_ = roll;
}

Vertex Angle::toVertex() const
{
	Vertex ret(1, 0, 0);
	//double yawrad = vertex_[YAW] * (M_PI / 180.0);
	//double rollrad = vertex_[ROLL] * (M_PI / 180.0);

	//ret.pitch(cos(yawrad) * cos(rollrad));
	//ret.yaw(sin(yawrad) * cos(rollrad));
	//ret.roll(sin(rollrad));

	auto rotMat = Matrix3d::rotmaty(pitch()) * Matrix3d::rotmatz(yaw()) * Matrix3d::rotmatx(roll());

	return ret.rotate(rotMat);
}

Matrix3d Angle::angleMatrix() const
{
	double sinpitch = sin(DEG2RAD(pitch())),
		cospitch = cos(DEG2RAD(pitch())),
		sinyaw = sin(DEG2RAD(yaw())),
		cosyaw = cos(DEG2RAD(yaw())),
		sinroll = sin(DEG2RAD(roll())),
		cosroll = cos(DEG2RAD(roll()));

	Matrix3d matrix;

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

Matrix3d Angle::calcRotation(const Angle& target) const
{
	return calcRotation(Quaternion{ target });
}

Matrix3d Angle::calcRotation(Quaternion target) const
{
	Quaternion curQuat(*this);
	curQuat.normalize();
	target.normalize();
	Quaternion rot = target * curQuat.conjugate();
	rot.normalize();
	return rot.quaternionMatrix();
}

void Angle::fromMatrix(const Matrix3d& matrix)
{
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

	if (!doubleeq(xyDist, 0))
	{
		pitch_ = RAD2DEG( atan2( -forward[2], xyDist ) );
		yaw_ = RAD2DEG( atan2( forward[1], forward[0] ) );
		roll_ = RAD2DEG( atan2( left[2], up[2] ) );
	}
	else
	{ // gimbal lock
		pitch_ = RAD2DEG( atan2( -forward[2], xyDist ) );
		yaw_ = RAD2DEG( atan2( -left[0], left[1] ) );
		roll_ = 0;
	}
}

std::string Angle::toStr() const
{
	std::ostringstream os;
	os << pitch_ << " "
		<< yaw_ << " "
		<< roll_;
	return os.str();
}

bool Angle::isAngle(const Angle& a)
{
	return (a.pitch() == a.pitch() && a.yaw() == a.yaw() && a.roll() == a.roll());
}

Angle& Angle::operator+=(const Angle& rhs)
{
	this->pitch_ += rhs.pitch_;
	this->yaw_ += rhs.yaw_;
	this->roll_ += rhs.roll_;
	return *this;
}

Angle& Angle::operator-=(const Angle& rhs)
{
	this->pitch_ -= rhs.pitch_;
	this->yaw_ -= rhs.yaw_;
	this->roll_ -= rhs.roll_;
	return *this;
}

Angle Angle::operator-() const
{
	Angle ret;
	if (pitch() > 0)
		ret.pitch(pitch() - 180.0);
	else
		ret.pitch(pitch() + 180.0);

	if (yaw() > 0)
		ret.yaw(yaw() - 180.0);
	else
		ret.yaw(yaw() + 180.0);

	if (roll() > 0)
		ret.roll(roll() - 180.0);
	else
		ret.roll(roll() + 180.0);

	return ret;
}

Angle::Angle(double pitch, double yaw, double roll) :
	pitch_(pitch),
	yaw_(yaw),
	roll_(roll)
{
}
