#include "stdafx.h"
#include "BoundingBox.h"
#include "Solid.h"

bool Solid::testCollision(const Solid &lhs, const Solid &rhs) {
	if(BoundingBox::testCollision(lhs.bbox(), rhs.bbox())) {
		for(const Side &lhss : lhs.sides) {
			for(const Side &rhss : rhs.sides) {
				if(Side::testCollision(lhss, rhss)) return true;
			}
		}
	}
	return false;
}

unsigned int Solid::parse(std::istream &stream) {
	unsigned int numparsed = 0;
	std::string curline;
	while(trim(curline) != "{") {
		std::getline(stream, curline);
		numparsed++;
	}
	unsigned int depth = 1;
	while (std::getline(stream, curline)) {
		numparsed++;
		if(trim(curline) == "side") {
			Side *side = new Side();
			numparsed += side->parse(stream);
			sides.put(side);
		} else if(trim(curline) == "editor") {
			numparsed+=edt.parse(stream);
		} else if(trim(curline) == "}") {
			if(--depth == 0) break;
		} else {
			KeyVal k(curline);
			if(k.key == "id") id_ = k.toInt();
		}
	}
	return numparsed;
}

void Solid::rotate(const Vertex &point, const Matrix &rotmat) {
	//mictimer rottimer("ms Solid::rotate()", 1000.0);
	for(Side &s : sides) {
		s.rotate(point, rotmat);
	}
}

void Solid::move(const Vector &vec) {
	for(Side &s : sides) {
		s.move(vec);
	}
}

void Solid::reID(unsigned int *solidID, unsigned int *sideID) {
	id_ = ++(*solidID);
	for(Side &side : sides) {
		side.reID(sideID);
	}
}

BoundingBox Solid::bbox() const {
	const double dmax = std::numeric_limits<double>::max();
	const double dmin = -dmax;
	Vertex min(dmax, dmax, dmax);
	Vertex max(dmin, dmin, dmin);
	for(const Side &s : sides) {
		BoundingBox sbox = s.bbox();
		min = Vertex::allmin(min, sbox.min);
		max = Vertex::allmax(max, sbox.max);
	}
	return BoundingBox(min, max);
}

Solid::Solid(void) : id_(0)
{
}

Solid::~Solid(void)
{
}
