#include "stdafx.h"
#include <sstream>
#include "Angle.h"
#include "BoundingBox.h"
#include "Entity.h"
#include "Matrix.h"
#include "Vector.h"
#include "Vertex.h"

unsigned int Entity::parse(std::istream &stream) {
	unsigned int numparsed = 0;
	std::string curline;
	while(trim(curline) != "{") {
		std::getline(stream, curline);
		numparsed++;
	}
	unsigned int depth = 1;
	while(std::getline(stream, curline)) {
		numparsed++;
		if(trim(curline) == "solid") {
			Solid *sol = new Solid();
			numparsed += sol->parse(stream);
			solids.put(sol);
		} else if (trim(curline) == "editor") {
			numparsed += edt.parse(stream);
		} else if(trim(curline) == "group") {
			Solid solid;
			numparsed += solid.parse(stream);
		} else if(trim(curline) == "}") {
			if(--depth == 0) break;
		} else if (trim(curline) == "{") {
			++depth;
		} else {
			keyvals.put(new KeyVal(curline));
		}
	}
	auto kv = keyvals.remove_first_match<std::string>("id", KeyVal::keycmp);
	if(kv != nullptr) {
		this->id_ = atoi(kv->val.c_str());
		delete kv;
	}
	return numparsed;
}

bool Entity::testCollision(const Entity &lhs, const Entity &rhs) {
	if(BoundingBox::testCollision(lhs.bbox(), rhs.bbox())) {
		for(const Solid &lhsolid : lhs.solids) {
			for(const Solid &rhsolid : rhs.solids) {
				 if(Solid::testCollision(lhsolid, rhsolid)) return true;
			}
		}
	}
	return false;
}

bool Entity::entclasscmp(const Entity &lhs, const std::string &rhs) {
	auto kv = lhs.keyvals.get_first_match<std::string>("classname", KeyVal::keycmp);
	if(kv == nullptr) return false;
	if(kv->val == rhs) return true;
	return false;
}

Entity Entity::defaultWorldEntity() {
    Entity world;
    world.id_ = 1;
    world.keyvals.put(new KeyVal("classname", "world"));
    world.keyvals.put(new KeyVal("mapversion", "1"));
    world.keyvals.put(new KeyVal("classname", "worldspawn"));
    world.keyvals.put(new KeyVal("detailmaterial", "detail/detailsprites"));
    world.keyvals.put(new KeyVal("detailvbsp", "detail.vbsp"));
    world.keyvals.put(new KeyVal("maxpropscreenwidth", "-1"));
    world.keyvals.put(new KeyVal("skyname", "sky_day01_01"));
    return world;
}

Vertex Entity::origin() {
	auto kv = keyvals.get_first_match<std::string>("origin", KeyVal::keycmp);
	if(kv != nullptr) return Vertex(kv->val);
	BoundingBox b = bbox();
	Vector vec(b.min, b.max);
	Vertex mod = vec.vec() * 0.5;
	return vec.beg()+mod;
}

Vertex Entity::originKV() {
	auto kv = keyvals.get_first_match<std::string>("origin", KeyVal::keycmp);
	if(kv != nullptr) return Vertex(kv->val);
	return Vertex();
}

Angle Entity::angles() {
	auto kv = keyvals.get_first_match<std::string>("angles", KeyVal::keycmp);
	if(kv != nullptr) return Angle(kv->val);
	return Angle();
}

void Entity::mergeSolids(const Entity &entity) {
	solids += entity.solids;
}

void Entity::rotate(const Matrix &rotmat, const Vertex &pt) {
	for(Solid &s : solids) {
		s.rotate(pt, rotmat);
	}
	Angle thAngle = angles();
	if(Angle::isAngle(thAngle)) {
		thAngle.fromMatrix(rotmat * thAngle.angleMatrix());
		(*this)["angles"] = thAngle.toStr();
	}
	Vertex orig = originKV();
	if(Vertex::isVertex(orig)) {
		orig = orig.rotate(pt, rotmat);
		(*this)["origin"] = orig.toStr();
	}
}

void Entity::move(const Vector &vec) {
	for(Solid &s : solids) {
		s.move(vec);
	}
	auto kv = keyvals.get_first_match<std::string>
		("origin", KeyVal::keycmp);
	if(kv != nullptr) {
		Vertex orig(kv->val);
		orig += vec.vec();
		kv->val = orig.toStr();
	}
}

void Entity::reID(unsigned int *entityID,
				  unsigned int *solidID,
				  unsigned int *sideID)
{
	id_ = ++(*entityID);
	for(Solid &solid : solids) {
		solid.reID(solidID, sideID);
	}
}

std::string &Entity::operator[](std::string key) {
	auto kv = keyvals.get_first_match(key, KeyVal::keycmp);
	if(kv == nullptr) {
		kv = new KeyVal(key);
		keyvals.put(kv);
	}
	return kv->val;
}

std::string Entity::operator[](std::string key) const {
	auto kv = keyvals.get_first_match(key, KeyVal::keycmp);
	if(kv == nullptr) return "";
	return kv->val;
}

BoundingBox Entity::bbox() const {
	const double dmax = std::numeric_limits<double>::max();
	const double dmin = -dmax;
	Vertex min(dmax, dmax, dmax);
	Vertex max(dmin, dmin, dmin);
	for(const Solid &s : solids) {
		BoundingBox sbox = s.bbox();
		min = Vertex::allmin(min, sbox.min);
		max = Vertex::allmax(max, sbox.max);
	}
	return BoundingBox(min, max);
}

Entity::Entity(void)
{
}

Entity::Entity(const std::string &classname) :
	id_(0)
{
	keyvals.put(new KeyVal("classname", classname));
}

Entity::~Entity(void)
{
}
