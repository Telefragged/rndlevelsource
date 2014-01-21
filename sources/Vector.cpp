#include "stdafx.h"
#include "Vector.h"

void Vector::rotate(const Matrix &rotmat) {
	_vec = _vec.rotate(rotmat);
}

Vector &Vector::operator+=(const Vector &rhs) {
	_vec+=rhs._vec;
	return *this;
}

Vector &Vector::operator-=(const Vector &rhs) {
	_vec-=rhs._vec;
	return *this;
}

Vector::Vector(void) :
	_orig(0.0, 0.0, 0.0),
	_vec(0.0, 0.0, 0.0)
{
}

Vector::Vector(const Vertex &vert) :
	_orig(vert),
	_vec(0.0, 0.0, 0.0)
{
}

Vector::Vector(const Vertex &beg, const Vertex &end) :
	_orig(beg),
	_vec(end-beg)
{
}

Vector::~Vector(void)
{
}

Vertex Vector::beg() const {
	return _orig;
}

Vertex Vector::end() const {
	return _orig + _vec;
}

Vertex Vector::vec() const {
	return _vec;
}

void Vector::vec(const Vertex &vec) {
	_vec = vec;
}
