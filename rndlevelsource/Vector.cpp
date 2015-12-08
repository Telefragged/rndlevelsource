#include "stdafx.h"
#include <string>
#include "Vector.h"

void Vector::rotate(const Matrix& rotmat)
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

std::string Vector::toStr() const
{
	return std::string() + '(' + _orig.toStr() + ") + t(" + _vec.toStr() + ')';
}

Vector::Vector(void) :
	_orig(0.0, 0.0, 0.0),
	_vec(0.0, 0.0, 0.0)
{
}

Vector::Vector(const Vertex& vert) :
	_orig(vert),
	_vec(0.0, 0.0, 0.0)
{
}

Vector::Vector(const Vertex& beg, const Vertex& end) :
	_orig(beg),
	_vec(end - beg)
{
}

Vector Vector::allMinMax(const Vertex& v1, const Vertex& v2)
{
	return Vector(Vertex::allmin(v1, v2), Vertex::allmax(v1, v2));
}

Vector::~Vector(void)
{
}

Vertex Vector::beg() const
{
	return _orig;
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

