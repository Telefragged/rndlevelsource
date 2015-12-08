#include "stdafx.h"
#include <ios>
#include <stdio.h>
#include "BoundingBox.h"
#include "KeyVal.h"
#include "Side.h"
#include "Vector.h"

unsigned int Side::parse(std::istream &stream) {
	unsigned int numparsed = 0;
	std::string curline;
	while(trim(curline) != "{") {
		std::getline(stream, curline);
		numparsed++;
	}
	while (std::getline(stream, curline)) {
		numparsed++;
		if (trim(curline) == "dispinfo") {
			disp.parse(stream);
		}
		else if(trim(curline) == "}") {
			break;
		} else {
			KeyVal parsed(curline);
			keyvals[parsed.key] = parsed.val;
		}
	}
	popuvars();
	return numparsed;
}

void Side::reID(unsigned int *sideID) {
	id_ = ++(*sideID);
}

void Side::popuvars() {
	if (keyvals.count("id") > 0) {
		this->id_ = atoi(keyvals.at("id").c_str());
		keyvals.erase("id");
	}
	if (keyvals.count("plane") > 0) {
		this->p.parsestr(keyvals.at("plane"));
		keyvals.erase("plane");
	}
	if (keyvals.count("uaxis") > 0) {
		this->uaxis.parsestr(keyvals.at("uaxis"));
		keyvals.erase("uaxis");
	}
	if (keyvals.count("vaxis") > 0) {
		this->vaxis.parsestr(keyvals.at("vaxis"));
		keyvals.erase("vaxis");
	}
}

void Side::rotate(const Vertex &point, const Matrix &rotmat)
{
	//mictimer rotatetimer("ms Side::rotate()", 1000.0);
	p.p1 = p.p1.rotate(point, rotmat);
	p.p2 = p.p2.rotate(point, rotmat);
	p.p3 = p.p3.rotate(point, rotmat);
	uaxis.v = uaxis.v.rotate(rotmat);
	vaxis.v = vaxis.v.rotate(rotmat);

	disp.rotate(point, rotmat);
}

void Side::move(const Vector &v) {
	Vertex mov = v.vec();
	p.p1+=mov;
	p.p2+=mov;
	p.p3+=mov;
	uaxis.translate(mov);
	vaxis.translate(mov);
}

BoundingBox Side::bbox() const {
	Vertex min, max;
	min = Vertex::allmin(p.p1, p.p2);
	min = Vertex::allmin(min, p.p3);
	max = Vertex::allmax(p.p1, p.p2);
	max = Vertex::allmax(max, p.p3);
	return BoundingBox(min, max);
}

bool Side::testCollision(const Side &lhs, const Side &rhs) {
	return Plane::testCollision(lhs.p, rhs.p);
}

Side::Side(void) : id_(0)
{
}

Side::~Side(void)
{
}
