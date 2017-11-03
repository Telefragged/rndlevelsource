#include "stdafx.h"
#include <algorithm>
#include "Matrix.h"
#include "Vector.h"
#include "Vertex.h"

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

Vertex::Vertex(const std::string& str)
{
	parsestr(str);
}

const Vertex Vertex::unitX = { 1, 0, 0 };
const Vertex Vertex::unitY = { 0, 1, 0 };
const Vertex Vertex::unitZ = { 0, 0, 1 };

void Vertex::x(double x)
{
	vertex_[0] = x;
}

void Vertex::y(double y)
{
	vertex_[1] = y;
}

void Vertex::z(double z)
{
	vertex_[2] = z;
}

double Vertex::x() const
{
	return vertex_[0];
}

double Vertex::y() const
{
	return vertex_[1];
}

double Vertex::z() const
{
	return vertex_[2];
}

Vertex Vertex::rotate(const Matrix3d& rotmat) const
{
	auto th = this->toMat();
	auto res = rotmat * th;
	return Vertex(res.get(0, 0), res.get(1, 0), res.get(2, 0));
}

Vertex Vertex::rotate(const Vertex& point, const Matrix3d& rotmat) const
{
	Vector vec = Vector::diff(point, *this);
	vec.rotate(rotmat);
	return Vertex(vec.end());
}

void Vertex::rotateInPlace(const Vertex& point, const Matrix3d& rotmat)
{
	Vector vec = Vector::diff(point, *this);
	vec.rotate(rotmat);
	*this = vec.end();
}

void Vertex::rotateInPlace(const Matrix3d& rotmat)
{
	auto vecmat = toMat();
	auto res = rotmat * vecmat;
	x(res.get(0, 0));
	y(res.get(1, 0));
	z(res.get(2, 0));
}

double Vertex::length() const
{
	return sqrt(
		  x() * x()
		+ y() * y()
		+ z() * z());
}

double Vertex::length(const Vertex& v)
{
	return v.length();
}

double Vertex::dotProduct(const Vertex& lhs, const Vertex& rhs)
{
	return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z();
}

double Vertex::dotProduct2D(const Vertex& lhs, const Vertex& rhs)
{
	return lhs.x() * rhs.x() + lhs.y() * rhs.y();
}

Vertex Vertex::absolute(const Vertex& v)
{
	return { fabs(v.x()), fabs(v.y()), fabs(v.z()) };
}

Vertex Vertex::normalize(const Vertex& v)
{
	
	double len = v.length();
	if (doubleeq(len, 0.0)) return v;
	return {
		v.x() / len,
		v.y() / len,
		v.z() / len
	};
}

Vertex Vertex::normalize() const
{
	double len = length();
	if (doubleeq(len, 0.0)) return Vertex(*this);
	return Vertex(
		vertex_[0] / len,
		vertex_[1] / len,
		vertex_[2] / len);
}

Vertex Vertex::crossProduct(const Vertex& lhs, const Vertex& rhs)
{
	return Vertex(
		lhs.vertex_[1] * rhs.vertex_[2] - lhs.vertex_[2] * rhs.vertex_[1],
		lhs.vertex_[2] * rhs.vertex_[0] - lhs.vertex_[0] * rhs.vertex_[2],
		lhs.vertex_[0] * rhs.vertex_[1] - lhs.vertex_[1] * rhs.vertex_[0]);
}

void Vertex::parsestr(const std::string& pstr)
{
	std::string str = trim(pstr, " \t\r\n()");
	size_t fspos = str.find_first_of(' '), espos = str.find_last_of(' ');
	if (fspos == std::string::npos || espos == std::string::npos || fspos == espos)
	{
		vertex_[0] = std::numeric_limits<double>::quiet_NaN();
		vertex_[1] = std::numeric_limits<double>::quiet_NaN();
		vertex_[2] = std::numeric_limits<double>::quiet_NaN();
		return;
	}
	size_t pos = 0;
	std::string xstr = nextword(pos, str);
	std::string ystr = nextword(pos, str);
	std::string zstr = nextword(pos, str);
	vertex_[0] = atof(xstr.c_str());
	vertex_[1] = atof(ystr.c_str());
	vertex_[2] = atof(zstr.c_str());
}

Vertex& Vertex::operator=(const Vertex& rhs)
{
	this->vertex_[0] = rhs.vertex_[0];
	this->vertex_[1] = rhs.vertex_[1];
	this->vertex_[2] = rhs.vertex_[2];
	return *this;
}

Vertex& Vertex::operator+=(const Vertex& rhs)
{
	this->vertex_[0] += rhs.vertex_[0];
	this->vertex_[1] += rhs.vertex_[1];
	this->vertex_[2] += rhs.vertex_[2];
	return *this;
}

Vertex& Vertex::operator-=(const Vertex& rhs)
{
	this->vertex_[0] -= rhs.vertex_[0];
	this->vertex_[1] -= rhs.vertex_[1];
	this->vertex_[2] -= rhs.vertex_[2];
	return *this;
}

Vertex& Vertex::operator*=(double mod)
{
	vertex_[0] *= mod;
	vertex_[1] *= mod;
	vertex_[2] *= mod;
	return *this;
}

Vertex & Vertex::operator/=(double mod)
{
	vertex_[0] /= mod;
	vertex_[1] /= mod;
	vertex_[2] /= mod;
	return *this;
}

bool Vertex::operator==(const Vertex& other) const
{
	return doubleeq(x(), other.x()) && doubleeq(y(), other.y()) && doubleeq(z(), other.z());
}

bool Vertex::operator<(const Vertex& other) const
{
	if (!doubleeq(x(), other.x()) && x() < other.x())
		return true;

	if (!doubleeq(y(), other.y()) && y() < other.y())
		return true;

	if (!doubleeq(z(), other.z()) && z() < other.z())
		return true;

	return false;
}

bool Vertex::parallel(const Vertex& lhs, const Vertex& rhs)
{
	//crossproduct of parallel vectors is (0, 0, 0)
	Vertex pvert = crossProduct(normalize(lhs), normalize(rhs));
	return doubleeq(pvert.x(), 0.0)
		&& doubleeq(pvert.y(), 0.0)
		&& doubleeq(pvert.z(), 0.0);
}

bool Vertex::equals(const Vertex& lhs, const Vertex& rhs)
{
	return doubleeq(lhs.x(), rhs.x())
		&& doubleeq(lhs.y(), rhs.y())
		&& doubleeq(lhs.z(), rhs.z());
}

double Vertex::dotProduct(const Vertex& rhs) const
{
	return
		x() * rhs.x() +
		y() * rhs.y() +
		z() * rhs.z();
}

Vertex Vertex::closestAxis() const
{
	Vertex abs = absolute(*this);

	if (abs.x() >= abs.y() && abs.x() >= abs.z()) return unitX;
	if (abs.y() >= abs.z()) return unitY;
	return unitZ;
}

Vertex Vertex::crossProduct(const Vertex& rhs) const
{
	return Vertex(
		vertex_[1] * rhs.vertex_[2] - vertex_[2] * rhs.vertex_[1],
		vertex_[2] * rhs.vertex_[0] - vertex_[0] * rhs.vertex_[2],
		vertex_[0] * rhs.vertex_[1] - vertex_[1] * rhs.vertex_[0]);
}

double Vertex::operator[](unsigned int pos) const
{
	return vertex_[pos];
}

double& Vertex::operator[](unsigned int pos)
{
	return vertex_[pos];
}

Matrix<double, 3, 1> Vertex::toMat() const
{
	Matrix<double, 3, 1> ret;
	ret.set(0, 0, x());
	ret.set(1, 0, y());
	ret.set(2, 0, z());
	return ret;
}

std::string Vertex::toStr() const
{
	std::ostringstream os;
	os << x() << " " << y() << " " << z();
	return os.str();
}

Vertex Vertex::allmin(const Vertex& v1, const Vertex& v2)
{
	Vertex v;
	v.x(std::min(v1.x(), v2.x()));
	v.y(std::min(v1.y(), v2.y()));
	v.z(std::min(v1.z(), v2.z()));
	return v;
}

Vertex Vertex::allmax(const Vertex& v1, const Vertex& v2)
{
	Vertex v;
	v.x(std::max(v1.x(), v2.x()));
	v.y(std::max(v1.y(), v2.y()));
	v.z(std::max(v1.z(), v2.z()));
	return v;
}


bool Vertex::isVertex(const Vertex& v)
{
	return (v.x() == v.x() && v.y() == v.y() && v.z() == v.z());
}

Vertex::~Vertex(void)
{
}

