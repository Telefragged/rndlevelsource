#pragma once

#include <string>
#include <string_view>

enum
{
	PITCH = 0,
	YAW,
	ROLL
};

class Quaternion;
class Vertex;
template <class _Ty, size_t _X, size_t _Y> class Matrix;

using Matrix3d = Matrix<double, 3, 3>;

class Angle
{
	double pitch_, yaw_, roll_;
public:
	//Returns a string representing the angle object.
	std::string toStr() const;

	//Returns true if all angles are non-NAN, return false otherwise.
	static bool isAngle(const Angle& a);

	double pitch() const;
	void pitch(double pitch);

	double yaw() const;
	void yaw(double yaw);

	double roll() const;
	void roll(double roll);

	//Returns a (non-normalized) vector representing the angle.
	Vertex toVertex() const;

	//Returns a rotation matrix defined by the pitch, yaw and roll of the angle.
	Matrix3d angleMatrix() const;

	//Calculate rotation matrix from this angle to target angle.
	Matrix3d calcRotation(const Angle& target) const;

	Matrix3d calcRotation(Quaternion target) const;

	//Sets the pitch, yaw and roll of the angle to the values represented by the rotation matrix.
	void fromMatrix(const Matrix3d& matrix);

	Angle& operator+=(const Angle& rhs);
	Angle& operator-=(const Angle& rhs);
	Angle operator-() const;

	Angle() = default;
	Angle(std::string_view str);
	Angle(const Matrix3d& mat);
	Angle(double pitch, double yaw, double roll);

	~Angle() = default;
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
