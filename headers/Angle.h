#pragma once
#include "Vertex.h"

enum {
	PITCH = 0,
	YAW,
	ROLL
};

class Angle :
	public Vertex
{
public:
	//Returns a string representing the angle object.
	std::string toStr() const;

	//Returns true if all angles are non-NAN, return false otherwise.
	static bool isAngle(const Angle &);

	void x(double x);
	void y(double y);
	void z(double z);

	double x() const;
	double y() const;
	double z() const;

	inline double pitch() const;
	inline double yaw() const;
	inline double roll() const;

	//Returns a (non-normalized) vector representing the angle.
	Vertex toVertex() const;

	//Returns a rotation matrix defined by the pitch, yaw and roll of the angle.
	Matrix angleMatrix() const;

	//Calculate rotation matrix from this angle to target angle.
	Matrix calcRotation(const Angle &);

	//Sets the pitch, yaw and roll of the angle to the values represented by the rotation matrix.
	void fromMatrix(const Matrix &);

	Angle &operator+=(const Angle &);
	Angle &operator-=(const Angle &);
	Angle operator-(void);

	Angle(void);
	Angle(std::string);
	Angle(double pitch, double yaw, double roll);
	~Angle(void);
};

inline Angle operator+(Angle lhs, const Angle &rhs) {
	lhs+=rhs;
	return lhs;
}

inline Angle operator-(Angle lhs, const Angle &rhs) {
	lhs-=rhs;
	return lhs;
}
