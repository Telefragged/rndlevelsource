#include "stdafx.h"
#include <fstream>
#include <string>
#include <stdio.h>
#include "Angle.h"
#include "BoundingBox.h"
#include "KeyVal.h"
#include "Matrix.h"
#include "Part.h"
#include "Vector.h"

unsigned int Part::parse(std::istream &stream) {
	unsigned int numparsed = 0;
	std::string curline;
	//while(trim(curline) != "world") {
	//	std::getline(file, curline);
	//	numparsed++;
	//}
	while(std::getline(stream, curline)) {
		numparsed++;
		if(trim(curline) == "world") {
			Entity *world = new Entity("world");
			numparsed += world->parse(stream);
			entities.put(world);
		} else if(trim(curline) == "entity") {
			Entity *entity = new Entity();
			numparsed+=entity->parse(stream);
			if((*entity)["classname"].substr(0, 12) == "rnd_connect_") {
				connections.put(new Connection(std::move(*entity)));
				delete entity;
			} else {
				entities.put(entity);
			}
		}
	}
	reID();
	return numparsed;
}

unsigned int Part::parse(std::string filepath) {
	std::ifstream file(filepath);
	if(!file.good()) {
		printf("Failed to open file: %s\n", filepath.c_str());
		return 0;
	}
	size_t bufsize = 128*1024;
	char *buf = new char[bufsize];
	file.rdbuf()->pubsetbuf(buf, bufsize);
	unsigned int ret = parse(file);
	file.close();
	delete [] buf;
	return ret;
}

BoundingBox Part::bbox() const {
	Entity *worldptr = entities.get_first_match<std::string>("world", &Entity::entclasscmp);
	BoundingBox ret;
	if(worldptr == nullptr) return ret;
	ret = worldptr->bbox();
	return ret;
}

bool Part::testCollision(const Part &lhs, const Part &rhs) {
	BoundingBox lhbbox = lhs.bbox(), rhbbox = rhs.bbox();
	if(BoundingBox::testCollision(lhbbox, rhbbox)) {
		Entity *lhworld = lhs.entities.get_first_match<std::string>("world", Entity::entclasscmp),
			*rhworld = rhs.entities.get_first_match<std::string>("world", Entity::entclasscmp);
		if(lhworld == nullptr || rhworld == nullptr) return false;
		return Entity::testCollision(*lhworld, *rhworld);
	}
	return false;
}

void Part::move(const Vector &vec) {
	if(!Vertex::isVertex(vec.vec())) return;
	for(Entity &e : entities) {
		e.move(vec);
	}
	for(Connection &c : connections) {
		c.move(vec);
	}
}

void Part::moveTo(const Vertex &pt) {
	Entity *worldptr = entities.get_first_match<std::string>("world", &Entity::entclasscmp);
	if(worldptr == nullptr) return;
	Vertex wOrig = worldptr->origin();
	Vector mov(wOrig, pt);
	move(mov);
}

void Part::rotate(const Angle &angle, const Vertex &pt) {
	Entity *worldptr = entities.get_first_match<std::string>
		("world", &Entity::entclasscmp);
	Vertex orig;
	if(Vertex::isVertex(pt)) orig = pt;
	else if(worldptr == nullptr) orig = Vertex(0, 0, 0);
	else orig = worldptr->origin();
	Angle rotangle(-angle[PITCH], angle[YAW], angle[ROLL]); //Invert pitch for compliance with hammer.
	Matrix rotmat = rotangle.angleMatrix();
	for(Entity &e : entities) {
		e.rotate(rotmat, orig);
	}
	for(Connection &c : connections) {
		c.rotate(rotmat, orig);
	}
}

std::streampos Part::toFile(std::string filename) {
	std::ofstream file(filename, std::ios::trunc);
	std::size_t bufsize = 128*1024;
	char *buf = new char[bufsize];
	file.rdbuf()->pubsetbuf(buf, bufsize);
	file<<*this;
	std::streampos fsize = file.tellp();
	file.close();
	delete [] buf;
	return fsize;
}

void Part::reID() {
	entityID_ = 0;
	solidID_ = 0;
	sideID_ = 0;
	for(Entity &e : entities) {
		e.reID(&entityID_, &solidID_, &sideID_);
	}
}

Part &Part::operator=(Part &&orig) {
	std::swap(this->entityID_, orig.entityID_);
	std::swap(this->solidID_, orig.solidID_);
	std::swap(this->sideID_, orig.sideID_);
	this->entities = std::move(orig.entities);
	this->connections = std::move(orig.connections);
	reID();
	return *this;
}

//Merges world entities and copies all entities
Part &Part::operator+=(const Part &rhs) {
	Entity *tWorld = entities.get_first_match<std::string>
		("world", Entity::entclasscmp),
		*origWorld = rhs.entities.get_first_match<std::string>
		("world", Entity::entclasscmp);
	if(tWorld != nullptr && origWorld != nullptr) {
		unsigned int fakeEntityID = 0;
		Entity worldcopy(*origWorld);
		worldcopy.reID(&fakeEntityID, &solidID_, &sideID_);
		tWorld->mergeSolids(worldcopy);
	}
	else if(tWorld == nullptr && origWorld != nullptr) {
		Entity *copy = new Entity(*origWorld);
		copy->reID(&entityID_, &solidID_, &sideID_);
		entities.put(copy);
	}
	unsigned int cap = std::numeric_limits<unsigned int>::max(), count = 0;
	if(this == &rhs) cap = entities.size(); //Safety against self-addition.
	for(auto &entity : rhs.entities) {		//We can't call LinkedList::operator+= as we have to not copy the world entity.
		if(count++ == cap) break;
		if(&entity == origWorld) continue;
		Entity *copy = new Entity(entity);
		copy->reID(&entityID_, &solidID_, &sideID_);
		entities.put(copy);
	}
	return *this;
}

//Returns reference to a entity with classname equal to argument.
//If no such entity exists, the function will create one and return its reference.
Entity &Part::operator[](std::string classname) {
	Entity *entity = entities.get_first_match<std::string>
		(classname, Entity::entclasscmp);
	if(entity == nullptr) {
		entity = new Entity(classname);
		entities.put(entity);
	}
	return *entity;
}

Part::Part(void)
{
	entityID_ = 0;
	solidID_ = 0;
	sideID_ = 0;
}

Part::Part(std::string filepath) {
	parse(filepath);
}

Part::Part(const Part &orig) :
	entityID_(orig.entityID_),
	solidID_(orig.solidID_),
	sideID_(orig.sideID_),
	entities(orig.entities),
	connections(orig.connections)
{
}

Part::Part(Part &&orig) :
	entityID_(orig.entityID_),
	solidID_(orig.solidID_),
	sideID_(orig.sideID_),
	entities(std::move(orig.entities)),
	connections(std::move(orig.connections))
{
}

Part::~Part(void)
{
}
