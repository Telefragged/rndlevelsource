#include "stdafx.h"
#include <assert.h>
#include <vector>
#include <algorithm>
#include "Matrix.h"
#include "Plane.h"
#include "utils.h"
#include "Vector.h"

void Plane::parsestr(std::string pstr) {
	unsigned int spos = 0, epos = 0;
	spos = pstr.find_first_of('(', epos);
	epos = pstr.find_first_of(')', epos)+1;
	p1.parsestr(pstr.substr(spos, epos));

	spos = pstr.find_first_of('(', epos);
	epos = pstr.find_first_of(')', epos)+1;
	p2.parsestr(pstr.substr(spos, epos));

	spos = pstr.find_first_of('(', epos);
	epos = pstr.find_first_of(')', epos)+1;
	p3.parsestr(pstr.substr(spos, epos));
}

Vertex Plane::normal() const {
	Vertex v1 = p2 - p1;
	Vertex v2 = p3 - p1;
	return Vertex::normalize(Vertex::crossProduct(v1, v2));
}

Matrix Plane::equation() const {
	Matrix ret(1, 4);
	Vertex norm = normal();
	double dist = Vertex::dotProduct(norm, p1);
	ret[0][0] = norm[0];
	ret[0][1] = norm[1];
	ret[0][2] = norm[2];
	ret[0][3] = dist;
	return ret;
}

Plane Plane::vectorPlane(const Vector &line1, const Vector &line2) {
    if(Vertex::parallel(line1.vec(), line2.vec())) return Plane();
    Vertex p1 = line1.beg();
    Vertex p2 = line1.end();
    Vertex p3 = line2.beg();
    if(Vertex::equals(p3, p1) || Vertex::equals(p3, p2)) p3 = line2.end();
    Plane ret;
    ret.p1 = p1;
    ret.p2 = p2;
    ret.p3 = p3;
    return ret;
}

Vector Plane::intersectLine(const Matrix &lhs, const Matrix &rhs) {
	Vertex lhsnorm(lhs[0][0], lhs[0][1], lhs[0][2]);
	Vertex rhsnorm(rhs[0][0], rhs[0][1], rhs[0][2]);
	// Check if planes are parallel
	if(Vertex::parallel(lhsnorm, rhsnorm)) return Vector(Vertex());
	Vertex interline = Vertex::crossProduct(lhsnorm, rhsnorm).normalize();
	double eqarr[2][3];
	char parcase = -1; // stores how points are stored and read
	// Test if the intersection line is parallel to the XY, XZ and YZ planes.
	if(!doubleeq(Vertex::dotProduct(interline, Vertex(0, 0, 1)), 0.0)) {
        // line will cross XY plane, z will be = 0
        eqarr[0][0] = lhs[0][0]; eqarr[0][1] = lhs[0][1]; eqarr[0][2] = lhs[0][3];
        eqarr[1][0] = rhs[0][0]; eqarr[1][1] = rhs[0][1]; eqarr[1][2] = rhs[0][3];
        parcase = 0;
	} else if(!doubleeq(Vertex::dotProduct(interline, Vertex(0, 1, 0)), 0.0)) {
        // line will cross XZ plane, y will be = 0
        eqarr[0][0] = lhs[0][0]; eqarr[0][1] = lhs[0][2]; eqarr[0][2] = lhs[0][3];
        eqarr[1][0] = rhs[0][0]; eqarr[1][1] = rhs[0][2]; eqarr[1][2] = rhs[0][3];
        parcase = 1;
	} else {
        // line will cross YZ plane, x will be = 0
        eqarr[0][0] = lhs[0][1]; eqarr[0][1] = lhs[0][2]; eqarr[0][2] = lhs[0][3];
        eqarr[1][0] = rhs[0][1]; eqarr[1][1] = rhs[0][2]; eqarr[1][2] = rhs[0][3];
        parcase = 2;
	}
	Matrix eq = toMat(eqarr);
	Matrix res = Matrix::gaussElim(eq);
	if(res.rows() == 0) return Vector(Vertex());
	Vertex pt;
	switch(parcase) {
    case 0:
        // z = 0;
        pt = Vertex(res[0][0], res[1][0], 0.0);
        break;
    case 1:
        // y = 0;
        pt = Vertex(res[0][0], 0.0, res[1][0]);
        break;
    case 2:
        // x = 0;
        pt = Vertex(0.0, res[0][0], res[1][0]);
        break;
	}
	Vector vec(pt);
	vec.vec(interline);
	// printf("Res: (%s) +t(%s)\n", vec.beg().toStr().c_str(), vec.vec().toStr().c_str());
	return vec;
}

double Plane::dist(const Vertex &pt, const Vector &line) {
	Vertex ptv(line.beg() - pt);
	Vertex crpd = Vertex::crossProduct(ptv, line.vec());
	return crpd.length() / line.vec().length();
}

double Plane::dist(const Vertex &pt, const Vertex &to) {
	return (pt-to).length();
}

bool Plane::crossesLine(const Plane &p, const Vector &line) {
    // We make the assumption that the line lies on the plane.
    Vector line2(line.beg());
    line2.vec(Vertex::normalize(p.normal()));
    // Construct plane given the argument line and the argument plane normal.
    // If the line lies on the plane, the constructed plane
    // is perpendicular to the argument plane.
    Plane pnorm = Plane::vectorPlane(line, line2);
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
	auto determineSide = [](double det) -> int { 

		if (doubleeq(det, 0.0)) return 0;	// Middle
		else if (det > 0.0) return 1;		// right
		else return -1;						// left
	};

	std::vector<int> points; // Hold the three point states

	// Huge thanks to Eirik Hjorthaug Kiil who helped me squash an annoying bug with the collision detection!

	detmat.setRow(2, p.p1 - pnorm.p1);
	points.push_back(determineSide(detmat.det()));

	detmat.setRow(2, p.p2 - pnorm.p1);
	points.push_back(determineSide(detmat.det()));

	detmat.setRow(2, p.p3 - pnorm.p1);
	points.push_back(determineSide(detmat.det()));

	auto it = std::remove(points.begin(), points.end(), 0); // remove the zeroes as we don't care if a point lies on a plane

	int prev = 0;
	
	for (auto cur = points.begin(); cur != it; ++cur) {
		if (prev != 0 && *cur != prev)
			return true;
		prev = *cur;
	}

    // It might not be the most efficient way of doing it, but it works.
	return false;
}

bool Plane::testCollision(const Plane &lhs, const Plane &rhs) {
	// Line where the infinite planes meet
	Vector vec = intersectLine(lhs.equation(), rhs.equation());
	// If planes are parallel beg() is set to NaN by intersectLine()
	Vertex line = vec.vec();
	// printf("%s\n", line.toStr().c_str());
	if(doubleeq(line.x(), 0.0) && doubleeq(line.y(), 0.0) && doubleeq(line.z(), 0.0)) return false;
	if(!Vertex::isVertex(vec.beg())) return false;
	// Determine which planes cross the intersection
	bool lhscrosses = Plane::crossesLine(lhs, vec);
	bool rhscrosses = Plane::crossesLine(rhs, vec);
	// If both planes cross the line then the planes collide
	if( lhscrosses && rhscrosses ) {
        printf("Collision: (%s, %s) \nline: %s\n\n", lhs.toStr().c_str(), rhs.toStr().c_str(), vec.toStr().c_str());
		printf("Plane equations: (%s, %s)\n\n", lhs.equation().toStr().c_str(), rhs.equation().toStr().c_str());
		return true;
	}
	return false;
}

std::string Plane::toStr() const {
	std::string ret;
	ret += '(' + p1.toStr() + ") (" + p2.toStr() + ") (" + p3.toStr() + ')';
	return ret;
}

Plane::Plane(void)
{
}

Plane::~Plane(void)
{
}
