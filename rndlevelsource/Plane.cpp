#include <vector>
#include "Matrix.h"
#include "Plane.h"
#include "utils.h"
#include "Vector.h"

void Plane::parsestr(std::string_view pstr)
{
	size_t spos, epos = 0;
	spos = pstr.find_first_of('(', epos);
	epos = pstr.find_first_of(')', epos) + 1;
	p1_.parsestr(pstr.substr(spos, epos));

	spos = pstr.find_first_of('(', epos);
	epos = pstr.find_first_of(')', epos) + 1;
	p2_.parsestr(pstr.substr(spos, epos));

	spos = pstr.find_first_of('(', epos);
	epos = pstr.find_first_of(')', epos) + 1;
	p3_.parsestr(pstr.substr(spos, epos));
}

Vertex Plane::normal() const
{
	Vertex v1 = p2() - p1();
	Vertex v2 = p3() - p1();
	return Vertex::normalize(Vertex::crossProduct(v2, v1));
}

Matrix<double, 1, 4> Plane::equation() const
{
	Matrix<double, 1, 4> ret;
	Vertex norm = normal();
	ret[0][0] = norm[0];
	ret[0][1] = norm[1];
	ret[0][2] = norm[2];
	ret[0][3] = -Vertex::dotProduct(norm, p1());
	return ret;
}

int Plane::evaluate(const Vertex& point) const
{
	double dist = Plane::dist(*this, point);
	if (doubleeq(dist, 0)) return 0;
	if (dist > 0) return 1;
	return -1;
}

Vertex Plane::closestAxisToNormal() const
{
	auto norm = Vertex::absolute(normal());

	if (norm.x() >= norm.y() && norm.x() >= norm.z())
		return Vertex::unitX;

	if (norm.y() >= norm.z())
		return Vertex::unitY;

	return Vertex::unitZ;
}

Plane Plane::vectorPlane(const Vector& line1, const Vector& line2)
{
	if (Vertex::parallel(line1.vec(), line2.vec()))
		return Plane();

	Vertex p1 = line1.beg();
	Vertex p2 = line1.end();
	Vertex p3 = !(Vertex::equals(line2.beg(), p1) || Vertex::equals(line2.beg(), p2)) ? line2.beg() : line2.end();

	return Plane(p1, p2, p3);
}

Plane Plane::flip(const Plane& plane)
{
	return Plane(plane.p3(), plane.p2(), plane.p1());
}

Vector Plane::intersectLine(const Plane& lhs, const Plane& rhs)
{
	auto lhsEq = lhs.equation();
	auto rhsEq = rhs.equation();

	Vertex lhsNorm(lhsEq[0][0], lhsEq[0][1], lhsEq[0][2]);
	Vertex rhsNorm(rhsEq[0][0], rhsEq[0][1], rhsEq[0][2]);

	double lhsDist = lhsEq[0][3];
	double rhsDist = rhsEq[0][3];

	Vertex interLine = Vertex::crossProduct(lhsNorm, rhsNorm);

	Vertex commonPoint;

	if(!doubleeq(interLine.x(), 0))
	{
		commonPoint.x(0);
		commonPoint.y((rhsNorm.z() * lhsDist - lhsNorm.z() * rhsDist) / interLine.x());
		commonPoint.z((-rhsNorm.y() * lhsDist + lhsNorm.y() * rhsDist) / interLine.x());
	}
	else if (!doubleeq(interLine.y(), 0))
	{
		commonPoint.x((rhsNorm.z() * lhsDist - lhsNorm.z() * rhsDist) / interLine.y());
		commonPoint.y(0);
		commonPoint.z((-rhsNorm.x() * lhsDist + lhsNorm.x() * rhsDist) / interLine.y());
	}
	else if (!doubleeq(interLine.z(), 0))
	{
		commonPoint.x((rhsNorm.y() * lhsDist - lhsNorm.y() * rhsDist) / interLine.z());
		commonPoint.y((-rhsNorm.x() * lhsDist + lhsNorm.x() * rhsDist) / interLine.z());
		commonPoint.z(0);
	}
	else
	{
		return Vector();
	}
	
	Vector ret(commonPoint, interLine.normalize());
	return ret;
}

Vertex Plane::intersectPoint(const Plane& p, const Vector& line)
{
	Vertex normal = p.normal();

	double num = -normal.dotProduct(line.beg() - p.p1());
	double denom = normal.dotProduct(line.vec());

	if (doubleeq(denom, 0))
		return Vertex();

	double res = num / denom;

	return line.beg() + res * line.vec();
}

double Plane::dist(const Plane& plane, const Vertex& point)
{
	auto eq = plane.equation();
	Vertex normal(eq[0][0], eq[0][1], eq[0][2]);

	//return (point.dotProduct(normal) - eq[0][3]) / normal.length();

	return normal.dotProduct(point) + eq[0][3];
}

double Plane::dist(const Vector& line, const Vertex& point)
{
	Vertex ptv(line.beg() - point);
	Vertex crpd = Vertex::crossProduct(ptv, line.vec());
	return crpd.length() / line.vec().length();
}

double Plane::dist(const Vertex& point, const Vertex& other)
{
	return (point - other).length();
}

std::string Plane::toStr() const
{
	std::string ret;
	ret += '(' + p1().toStr() + ") (" + p2().toStr() + ") (" + p3().toStr() + ')';
	return ret;
}


Plane::Plane(const std::string& str)
{
	this->parsestr(str);
}

Plane::Plane(const Vertex& p1, const Vertex& p2, const Vertex& p3) : p1_(p1), p2_(p2), p3_(p3)
{
}

