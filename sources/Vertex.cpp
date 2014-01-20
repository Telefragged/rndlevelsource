#include "stdafx.h"
#include <assert.h>
#include <iostream>
#include <math.h>
#include "Matrix.h"
#include "Vertex.h"
#include "Vector.h"

Vertex::Vertex(void)
{

	vertex_[0] = std::numeric_limits<double>::quiet_NaN();
	vertex_[1] = std::numeric_limits<double>::quiet_NaN();
	vertex_[2] = std::numeric_limits<double>::quiet_NaN();
}

Vertex::Vertex(double x, double y, double z)
{
	vertex_[0] = x;
	vertex_[1] = y;
	vertex_[2] = z;
}

Vertex::Vertex(const std::string &str) {
	parsestr(str);
}

void Vertex::x(double x) {vertex_[0] = x;};
void Vertex::y(double y) {vertex_[1] = y;};
void Vertex::z(double z) {vertex_[2] = z;};

double Vertex::x() const {return vertex_[0];}
double Vertex::y() const {return vertex_[1];}
double Vertex::z() const {return vertex_[2];}

Vertex Vertex::rotate(const Matrix &rotmat) {
	Matrix th = this->toMat();
	Matrix res = rotmat * th;
	return Vertex(res.get(0, 0), res.get(1, 0), res.get(2, 0));
}

Vertex Vertex::rotate(const Vertex &point, const Matrix &rotmat) {
	Vector vec(point, *this);
	vec.rotate(rotmat);
	return Vertex(vec.end());
}

void Vertex::rotateInPlace(const Vertex &point, const Matrix &rotmat) {
	Vector vec(point, *this);
	Matrix vecmat = vec.vec().toMat();
	Matrix res = rotmat * vecmat;
	x(res.get(0, 0));
	y(res.get(1, 0));
	z(res.get(2, 0));
}

void Vertex::rotateInPlace(const Matrix &rotmat) {
	Matrix vecmat = toMat();
	Matrix res = rotmat * vecmat;
	x(res.get(0, 0));
	y(res.get(1, 0));
	z(res.get(2, 0));
}

double Vertex::length() const {
	return sqrt(
		  (x() * x())
		+ (y() * y())
		+ (z() * z()));
}

double Vertex::length(const Vertex &v) {
	return v.length();
}

double Vertex::dotProduct(const Vertex &lhs, const Vertex &rhs) {
	double ret = 0.0;
	ret += lhs.x() * rhs.x();
	ret += lhs.y() * rhs.y();
	ret += lhs.z() * rhs.z();
	return ret;
}

Vertex Vertex::normalize(const Vertex &v) {
	double len = v.length();
	return Vertex(
		v.x() / len,
		v.y() / len,
		v.z() / len);
}

Vertex Vertex::normalize() {
	double len = length();
	return Vertex(
		vertex_[0] / len,
		vertex_[1] / len,
		vertex_[2] / len);
}

Vertex Vertex::crossProduct(const Vertex &lhs, const Vertex &rhs) {
	return Vertex(
		(lhs.vertex_[1] * rhs.vertex_[2]) - (lhs.vertex_[2] * rhs.vertex_[1]),
		(lhs.vertex_[2] * rhs.vertex_[0]) - (lhs.vertex_[0] * rhs.vertex_[2]),
		(lhs.vertex_[0] * rhs.vertex_[1]) - (lhs.vertex_[1] * rhs.vertex_[0]));
}

void Vertex::parsestr(const std::string &pstr) {
	std::string str = trim(pstr, " \t\n()");
	unsigned int fspos = str.find_first_of(' '), espos = str.find_last_of(' ');
	if(fspos == std::string::npos || espos == std::string::npos || fspos == espos) {
		vertex_[0] = std::numeric_limits<double>::quiet_NaN();
		vertex_[1] = std::numeric_limits<double>::quiet_NaN();
		vertex_[2] = std::numeric_limits<double>::quiet_NaN();
		return;
	}
	unsigned int pos = 0;
	std::string xstr = nextword(pos, str);
	std::string ystr = nextword(pos, str);
	std::string zstr = nextword(pos, str);
	vertex_[0] = atof(xstr.c_str());
	vertex_[1] = atof(ystr.c_str());
	vertex_[2] = atof(zstr.c_str());
}

Vertex &Vertex::operator=(const Vertex &rhs) {
	this->vertex_[0] = rhs.vertex_[0];
	this->vertex_[1] = rhs.vertex_[1];
	this->vertex_[2] = rhs.vertex_[2];
	return *this;
}

Vertex &Vertex::operator+=(const Vertex &rhs) {
	this->vertex_[0] += rhs.vertex_[0];
	this->vertex_[1] += rhs.vertex_[1];
	this->vertex_[2] += rhs.vertex_[2];
	return *this;
}

Vertex &Vertex::operator-=(const Vertex &rhs) {
	this->vertex_[0] -= rhs.vertex_[0];
	this->vertex_[1] -= rhs.vertex_[1];
	this->vertex_[2] -= rhs.vertex_[2];
	return *this;
}

Vertex &Vertex::operator*=(double mod) {
	vertex_[0]*=mod;
	vertex_[1]*=mod;
	vertex_[2]*=mod;
	return *this;
}

bool Vertex::parallel(const Vertex &lhs, const Vertex &rhs) {
    //crossproduct of parallel vectors is (0, 0, 0)
    Vertex pvert = Vertex::crossProduct(lhs, rhs);
    return (doubleeq(pvert.x(), 0.0))
        && (doubleeq(pvert.y(), 0.0))
        && (doubleeq(pvert.z(), 0.0));
}

double Vertex::dotProduct(const Vertex &rhs) {
	return
		(x() * rhs.x()) +
		(y() * rhs.y()) +
		(z() * rhs.z());
}

Vertex Vertex::crossProduct(const Vertex &rhs) {
	return Vertex(
		(vertex_[1] * rhs.vertex_[2]) - (vertex_[2] * rhs.vertex_[1]),
		(vertex_[2] * rhs.vertex_[0]) - (vertex_[0] * rhs.vertex_[2]),
		(vertex_[0] * rhs.vertex_[1]) - (vertex_[1] * rhs.vertex_[0]));
}

double Vertex::operator[](unsigned int pos) const {
	return vertex_[pos];
}

double &Vertex::operator[](unsigned int pos) {
	return vertex_[pos];
}

Matrix Vertex::toMat() {
	Matrix ret(3, 1);
	ret.set(0, 0, x());
	ret.set(1, 0, y());
	ret.set(2, 0, z());
	return ret;
}

std::string Vertex::toStr() const {
	std::ostringstream os;
	os<<x()<<" "<<y()<<" "<<z();
	return os.str();
}

Vertex Vertex::allmin(const Vertex &v1, const Vertex &v2) {
	Vertex v;
	v.x(std::min(v1.x(), v2.x() ));
	v.y(std::min(v1.y(), v2.y() ));
	v.z(std::min(v1.z(), v2.z() ));
	return v;
}

Vertex Vertex::allmax(const Vertex &v1, const Vertex &v2) {
	Vertex v;
	v.x(std::max(v1.x(), v2.x() ));
	v.y(std::max(v1.y(), v2.y() ));
	v.z(std::max(v1.z(), v2.z() ));
	return v;
}


bool Vertex::isVertex(const Vertex &v) {
	return (v.x() == v.x() && v.y() == v.y() && v.z() == v.z());
}

Vertex::~Vertex(void)
{
}
