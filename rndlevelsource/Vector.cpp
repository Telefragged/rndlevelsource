#include "Vector.h"

#include <string>

void Vector::rotate(const Matrix3d& rotmat)
{
	_vec = _vec.rotate(rotmat);
}

Vector& Vector::operator+=(const Vector& rhs)
{
	_vec += rhs._vec;
	return *this;
}

Vector& Vector::operator-=(const Vector& rhs)
{
	_vec -= rhs._vec;
	return *this;
}

Vector& Vector::operator*=(double rhs)
{
	_vec *= rhs;
	return *this;
}

Vector Vector::diff(const Vertex& p1, const Vertex& p2)
{
	Vector ret;
	ret.beg(p1);
	ret.vec(p2 - p1);
	return ret;
}

std::string Vector::toStr() const
{
	return '(' + _orig.toStr() + ") + t(" + _vec.toStr() + ')';
}

Vector::Vector()
{
}

Vector::Vector(const Vertex& vert) :
	_orig(0, 0, 0),
	_vec(vert)
{
}

Vector::Vector(double x, double y, double z) :
	_orig(0, 0, 0),
	_vec(x, y, z)
{
}

Vector::Vector(const Vertex& beg, const Vertex& vec) :
	_orig(beg),
	_vec(vec)
{
}

Vector Vector::allMinMax(const Vertex& v1, const Vertex& v2)
{
	return diff(Vertex::allmin(v1, v2), Vertex::allmax(v1, v2));
}

Vertex Vector::intersectPoint(const Vector& lhs, const Vector& rhs)
{
	Vertex eqLhs = Vertex::crossProduct(lhs.vec(), rhs.vec());
	Vertex eqRhs = Vertex::crossProduct(rhs.beg() - lhs.beg(), rhs.vec());

	if (doubleeq(eqLhs.length(), 0) || !Vertex::parallel(eqLhs, eqRhs))
		return Vertex();

	double a = eqRhs.length() / eqLhs.length();

	return lhs.beg() + a * lhs.vec();
}

Vector::~Vector(void)
{
}

Vertex Vector::beg() const
{
	return _orig;
}

void Vector::beg(const Vertex& beg)
{
	_orig = beg;
}

Vertex Vector::end() const
{
	return _orig + _vec;
}

Vertex Vector::vec() const
{
	return _vec;
}

void Vector::vec(const Vertex& vec)
{
	_vec = vec;
}

