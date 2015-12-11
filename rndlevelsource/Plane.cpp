#include <vector>
#include <algorithm>
#include "Matrix.h"
#include "Plane.h"
#include "utils.h"
#include "Vector.h"

void Plane::parsestr(std::string pstr)
{
	unsigned int spos, epos = 0;
	spos = pstr.find_first_of('(', epos);
	epos = pstr.find_first_of(')', epos) + 1;
	p1.parsestr(pstr.substr(spos, epos));

	spos = pstr.find_first_of('(', epos);
	epos = pstr.find_first_of(')', epos) + 1;
	p2.parsestr(pstr.substr(spos, epos));

	spos = pstr.find_first_of('(', epos);
	epos = pstr.find_first_of(')', epos) + 1;
	p3.parsestr(pstr.substr(spos, epos));
}

Vertex Plane::normal() const
{
	Vertex v1 = p2 - p1;
	Vertex v2 = p3 - p1;
	return Vertex::normalize(Vertex::crossProduct(v1, v2));
}

Matrix Plane::equation() const
{
	Matrix ret(1, 4);
	Vertex norm = normal();
	double dist = Vertex::dotProduct(norm, p1);
	ret[0][0] = norm[0];
	ret[0][1] = norm[1];
	ret[0][2] = norm[2];
	ret[0][3] = dist;
	return ret;
}

Plane Plane::vectorPlane(const Vector& line1, const Vector& line2)
{
	if (Vertex::parallel(line1.vec(), line2.vec()))
		return Plane();

	Vertex p1 = line1.beg();
	Vertex p2 = line1.end();
	Vertex p3 = line2.beg();
	if (Vertex::equals(p3, p1) || Vertex::equals(p3, p2))
		p3 = line2.end();

	Plane ret;
	ret.p1 = p1;
	ret.p2 = p2;
	ret.p3 = p3;
	return ret;
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
	
	Vector ret(interLine.normalize());
	ret.beg(commonPoint);
	return ret;
}

double Plane::dist(const Vertex& pt, const Vector& line)
{
	Vertex ptv(line.beg() - pt);
	Vertex crpd = Vertex::crossProduct(ptv, line.vec());
	return crpd.length() / line.vec().length();
}

double Plane::dist(const Vertex& pt, const Vertex& to)
{
	return (pt - to).length();
}

bool Plane::crossesLine(const Plane& p, const Vector& line)
{
	// We make the assumption that the line lies on the plane.
	Vector line2(Vertex::normalize(p.normal()));
	line2.beg(line.beg());
	// Construct plane given the argument line and the argument plane normal.
	// If the line lies on the plane, the constructed plane
	// is perpendicular to the argument plane.
	Plane pnorm = vectorPlane(line, line2);
	Vertex row1 = pnorm.p3 - pnorm.p1,
		row2 = pnorm.p2 - pnorm.p1;
	// Construct matrix with vectors AB and AC on the new plane
	// and the point we want test. The sign of the determinant
	// determines which side of the plane the point lies on.
	Matrix detmat(3, 3);
	detmat.setRow(0, row1);
	detmat.setRow(1, row2);

	// Using this fact, we can determine if the three points
	// That define the plane are on the same side of the line or not.

	// this function determines which side the point lies on.
	auto determineSide = [](double det) -> int
		{
			if (doubleeq(det, 0.0)) return 0; // Middle
			else if (det > 0.0) return 1; // right
			else return -1; // left
		};

	std::vector<int> points(3); // Hold the three point states

	// Huge thanks to Eirik Hjorthaug Kiil who helped me squash an annoying bug with the collision detection!

	detmat.setRow(2, p.p1 - pnorm.p1);
	points.push_back(determineSide(detmat.det()));

	detmat.setRow(2, p.p2 - pnorm.p1);
	points.push_back(determineSide(detmat.det()));

	detmat.setRow(2, p.p3 - pnorm.p1);
	points.push_back(determineSide(detmat.det()));

	auto it = remove(points.begin(), points.end(), 0); // remove the zeroes as we don't care if a point lies on a plane

	int prev = 0;

	for (auto cur = points.begin(); cur != it; ++cur)
	{
		if (prev != 0 && *cur != prev)
			return true;
		prev = *cur;
	}

	// It might not be the most efficient way of doing it, but it works.
	return false;
}

bool Plane::testCollision(const Plane& lhs, const Plane& rhs)
{
	// Line where the infinite planes meet
	Vector vec = intersectLine(lhs, rhs);
	// If planes are parallel beg() is set to NaN by intersectLine()
	Vertex line = vec.vec();
	// printf("%s\n", line.toStr().c_str());
	if (!Vertex::isVertex(line)) return false;
	if (!Vertex::isVertex(vec.beg())) return false;
	// Determine which planes cross the intersection
	bool lhscrosses = crossesLine(lhs, vec);
	bool rhscrosses = crossesLine(rhs, vec);
	// If both planes cross the line then the planes collide
	if (lhscrosses && rhscrosses)
	{
		printf("Collision: (%s, %s) \nline: %s\n\n", lhs.toStr().c_str(), rhs.toStr().c_str(), vec.toStr().c_str());
		printf("Plane equations: (%s, %s)\n\n", lhs.equation().toStr().c_str(), rhs.equation().toStr().c_str());
		return true;
	}
	return false;
}

std::string Plane::toStr() const
{
	std::string ret;
	ret += '(' + p1.toStr() + ") (" + p2.toStr() + ") (" + p3.toStr() + ')';
	return ret;
}

Plane::Plane(void)
{
}

Plane::Plane(const std::string &str)
{
	this->parsestr(str);
}

Plane::Plane(const Vertex &p1, const Vertex &p2, const Vertex &p3) : p1(p1), p2(p2), p3(p3)
{
}

Plane::~Plane(void)
{
}
