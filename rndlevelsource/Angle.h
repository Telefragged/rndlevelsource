#pragma once
#include "Vertex.h"

enum
{
	PITCH = 0,
	YAW,
	ROLL
};

class Angle :
	public Vertex
{
public:
	//Returns a string representing the angle object.
	std::string toStr() const override;

	//Returns true if all angles are non-NAN, return false otherwise.
	static bool isAngle(const Angle&);

	double pitch() const;
	double yaw() const;
	double roll() const;

	//Returns a (non-normalized) vector representing the angle.
	Vertex toVertex() const;

	//Returns a rotation matrix defined by the pitch, yaw and roll of the angle.
	Matrix3d angleMatrix() const;

	//Calculate rotation matrix from this angle to target angle.
	Matrix3d calcRotation(const Angle& target) const;

	//Sets the pitch, yaw and roll of the angle to the values represented by the rotation matrix.
	void fromMatrix(const Matrix3d& matrix);

	Angle& operator+=(const Angle& rhs);
	Angle& operator-=(const Angle& rhs);
	Angle operator-() const;

	Angle();
	Angle(const std::string&);
	Angle(double pitch, double yaw, double roll);

	virtual ~Angle();
};

inline Angle operator+(Angle lhs, const Angle& rhs)
{
	lhs += rhs;
	return lhs;
}

inline Angle operator-(Angle lhs, const Angle& rhs)
{
	lhs -= rhs;
	return lhs;
}

