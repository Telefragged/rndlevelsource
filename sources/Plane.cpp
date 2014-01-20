#include "stdafx.h"
#include <assert.h>
#include "Matrix.h"
#include "Plane.h"
#include "utils.h"
#include "Vector.h"

void Plane::parsestr(std::string pstr) {
	unsigned int spos = 0, epos = 0;
	spos = pstr.find_first_of('(', epos); epos = pstr.find_first_of(')', epos)+1;
	p1.parsestr(pstr.substr(spos, epos));
	spos = pstr.find_first_of('(', epos); epos = pstr.find_first_of(')', epos)+1;
	p2.parsestr(pstr.substr(spos, epos));
	spos = pstr.find_first_of('(', epos); epos = pstr.find_first_of(')', epos)+1;
	p3.parsestr(pstr.substr(spos, epos));
}

Vertex Plane::normal() const {
	Vertex v1 = p2 - p1;
	Vertex v2 = p3 - p1;
	return Vertex::crossProduct(v1, v2);
}

Matrix Plane::equation() const {
	Matrix ret(1, 4);
	Vertex norm = normal();
	double dist = -Vertex::dotProduct(norm, p1);
	ret[0][0] = norm[0];
	ret[0][1] = norm[1];
	ret[0][2] = norm[2];
	ret[0][3] = dist;
	return ret;
}

Vector Plane::intersectLine(const Matrix &lhs, const Matrix &rhs) {
	Vertex lhsnorm(lhs[0][0], lhs[0][1], lhs[0][2]);
	Vertex rhsnorm(rhs[0][0], rhs[0][1], rhs[0][2]);
	//Check if planes are parallel
	if(Vertex::parallel(lhsnorm, rhsnorm)) return Vector(Vertex());
	Vertex interline = Vertex::normalize(Vertex::crossProduct(lhsnorm, rhsnorm));
	double eqarr[2][3];
	int parcase = -1;
	//Test if the intersection line is parallel to the XY, XZ and YZ planes.
	if(!doubleeq(Vertex::dotProduct(interline, Vertex(0, 0, 1)), 0.0)) {
        //line will cross XY plane, z will be = 0
        eqarr[0][0] = lhs[0][0]; eqarr[0][1] = lhs[0][1]; eqarr[0][2] = lhs[0][3];
        eqarr[1][0] = rhs[0][0]; eqarr[1][1] = rhs[0][1]; eqarr[1][2] = rhs[0][3];
        parcase = 0;
	} else if(!doubleeq(Vertex::dotProduct(interline, Vertex(0, 1, 0)), 0.0)) {
        //line will cross XZ plane, y will be = 0
        eqarr[0][0] = lhs[0][0]; eqarr[0][1] = lhs[0][2]; eqarr[0][2] = lhs[0][3];
        eqarr[1][0] = rhs[0][0]; eqarr[1][1] = rhs[0][2]; eqarr[1][2] = rhs[0][3];
        parcase = 1;
	} else if(!doubleeq(Vertex::dotProduct(interline, Vertex(1, 0, 0)), 0.0)) {
        //line will cross YZ plane, x will be = 0
        eqarr[0][0] = lhs[0][1]; eqarr[0][1] = lhs[0][2]; eqarr[0][2] = lhs[0][3];
        eqarr[1][0] = rhs[0][1]; eqarr[1][1] = rhs[0][2]; eqarr[1][2] = rhs[0][3];
        parcase = 2;
	}
	//z = 0
	Matrix eq = toMat(eqarr);
	Matrix res = Matrix::gaussElim(eq);
	if(res.rows() == 0) return Vector(Vertex());
	Vertex pt;
	switch(parcase) {
    case 0:
        //z = 0;
        pt = Vertex(res[0][0], res[1][0], 0.0);
        break;
    case 1:
        //y = 0;
        pt = Vertex(res[0][0], 0.0, res[1][0]);
        break;
    case 2:
        //x = 0;
        pt = Vertex(0.0, res[0][0], res[1][0]);
        break;
	}
	Vector vec(pt);
	vec.vec(interline);
	//printf("Res: (%s) +t(%s)\n", vec.beg().toStr().c_str(), vec.vec().toStr().c_str());
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

inline bool oppositeLines(const Vertex &pt1, const Vertex &pt2, const Vector &line) {
	Vertex cpt1 = Vertex::crossProduct(line.vec(), pt1);
	Vertex cpt2 = Vertex::crossProduct(line.vec(), pt2);
	return Vertex::dotProduct(cpt1, cpt2) < 0.0;
}

bool Plane::crossesLine(const Plane &p, const Vector &line) {
	if(oppositeLines(p.p1, p.p2, line)) return true;
	if(oppositeLines(p.p1, p.p3, line)) return true;
	if(oppositeLines(p.p2, p.p3, line)) return true;
	return false;
	//std::pair<Vertex, double> max, mid, min;
	//max.first = p.p1; max.second = dist(p.p1, line);
	//mid.first = p.p2; mid.second = dist(p.p2, line);
	//if(mid.second > max.second) mid.swap(max);
	//min.first = p.p3; min.second = dist(p.p3, line);
	//if(min.second > mid.second) {
	//	min.swap(mid);
	//	if(mid.second > max.second) mid.swap(max);
	//}
	//if(Plane::dist(max.first, mid.first) > max.second
	//	&& !doubleeq(Plane::dist(max.first, mid.first), max.second)) {
	//	//printf("mid %f %f\n", Plane::dist(max.first, mid.first), max.second);
	//	return true;
	//}
	//else if(Plane::dist(max.first, min.first) > max.second
	//	&& !doubleeq(Plane::dist(max.first, min.first), max.second)) {
	//	//printf("min %f %f\n", Plane::dist(max.first, min.first), max.second);
	//	return true;
	//}
	//else return false;
}

bool Plane::testCollision(const Plane &lhs, const Plane &rhs) {
	//Line where the infinite planes meet
	Vector vec = intersectLine(lhs.equation(), rhs.equation());
	//If planes are parallel beg() is set to NaN
	Vertex line = vec.vec();
	//printf("%s\n", line.toStr().c_str());
	if(doubleeq(line.x(), 0.0) && doubleeq(line.y(), 0.0) && doubleeq(line.z(), 0.0)) return false;
	if(!Vertex::isVertex(vec.beg())) return false;
	//Determine which planes cross the intersection
	bool lhscrosses = Plane::crossesLine(lhs, vec);
	bool rhscrosses = Plane::crossesLine(rhs, vec);
	//If both cross the line then the planes collide
	printf("Collision: (%s, %s) \nline %s + t(%s)\n\n", lhs.toStr().c_str(), rhs.toStr().c_str(), vec.beg().toStr().c_str(), vec.vec().toStr().c_str());
	if( lhscrosses && rhscrosses ) {
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
