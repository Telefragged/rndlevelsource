#include "Vector.h"

#include <string>

void Vector::rotate(const Matrix3d& rotmat)
{
	vec_ = vec_.rotate(rotmat);
}

Vector& Vector::operator+=(const Vector& rhs)
{
	vec_ += rhs.vec_;
	return *this;
}

Vector& Vector::operator-=(const Vector& rhs)
{
	vec_ -= rhs.vec_;
	return *this;
}

Vector& Vector::operator*=(double rhs)
{
	vec_ *= rhs;
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
	return '(' + orig_.toStr() + ") + t(" + vec_.toStr() + ')';
}

Vector::Vector()
{
}

Vector::Vector(const Vertex& vert) :
	orig_(0, 0, 0),
	vec_(vert)
{
}

Vector::Vector(double x, double y, double z) :
	orig_(0, 0, 0),
	vec_(x, y, z)
{
}

Vector::Vector(const Vertex& beg, const Vertex& vec) :
	orig_(beg),
	vec_(vec)
{
}

void Vector::setLength(double length)
{
	vec_ = vec_.normalize() * length;
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

	if (eqRhs.normalize() != eqLhs.normalize())
		a = -a;

	return lhs.beg() + a * lhs.vec();
}

bool Vector::liesOnLine(const Vertex & v) const
{
	Vertex begToV = v - beg();

	Vertex line = vec();

	Vertex crpd = line.crossProduct(begToV);

	return line.crossProduct(begToV) == Vertex{0, 0, 0};
}

double Vector::calculatePosition(const Vertex & v) const
{
	if(!liesOnLine(v))
		return std::numeric_limits<double>::quiet_NaN();

	Vertex begToV = v - beg();

	double len = begToV.length() / vec().length();

	if (begToV.normalize() != vec().normalize())
		return -len;

	return len;
}

Vector::~Vector(void)
{
}

Vertex Vector::beg() const
{
	return orig_;
}

void Vector::beg(const Vertex& beg)
{
	orig_ = beg;
}

Vertex Vector::end() const
{
	return orig_ + vec_;
}

Vertex Vector::vec() const
{
	return vec_;
}

void Vector::vec(const Vertex& vec)
{
	vec_ = vec;
}

