#include <algorithm>
#include <fstream>
#include <functional>
#include <string>
#include <stdio.h>

#include "Angle.h"
#include "BoundingBox.h"
#include "DispInfo.h"
#include "Matrix.h"
#include "Part.h"
#include "Vector.h"

unsigned int Part::parse(std::istream& stream)
{
	unsigned int numparsed = 0;
	std::string curline;
	//while(trim(curline) != "world") {
	//	std::getline(file, curline);
	//	numparsed++;
	//}
	while (getline(stream, curline))
	{
		numparsed++;
		if (trim(curline) == "world")
		{
			auto it = entities.emplace(entities.end(), "world");
			numparsed += it->parse(stream);
		}
		else if (trim(curline) == "entity")
		{
			Entity entity;
			numparsed += entity.parse(stream);
			if (entity["classname"].substr(0, 12) == "rnd_connect_")
			{
				connections.push_back(std::move(entity));
			}
			else
			{
				entities.push_back(std::move(entity));
			}
		}
	}
	reID();
	return numparsed;
}

unsigned int Part::parse(std::string filepath)
{
	std::ifstream file(filepath);
	if (!file.good())
		throw std::exception("Failed to open file");

	unsigned int ret = parse(file);
	file.close();
	return ret;
}

unsigned int Part::countEntities(std::string classname) const
{
	unsigned int count = 0;
	for (const Entity& entity : entities)
		if (entity["classname"] == classname)
			count++;
	return count;
}

BoundingBox Part::bbox() const
{
	BoundingBox ret;
	auto it = find_if(entities.cbegin(), entities.cend(), &Entity::entworldcmp);
	if (it == entities.cend()) return ret;
	ret = it->bbox();
	return ret;
}

bool Part::testCollision(const Part& lhs, const Part& rhs)
{
	BoundingBox lhbbox = lhs.bbox(), rhbbox = rhs.bbox();
	if (BoundingBox::testCollision(lhbbox, rhbbox))
	{
		auto lhsit = find_if(lhs.entities.cbegin(), lhs.entities.cend(), &Entity::entworldcmp),
			rhsit = find_if(rhs.entities.cbegin(), rhs.entities.cend(), &Entity::entworldcmp);
		if (lhsit == lhs.entities.cend() || rhsit == rhs.entities.cend()) return false;
		return Entity::testCollision(*lhsit, *rhsit);
	}
	return false;
}

void Part::move(const Vector& vec)
{
	if (!Vertex::isVertex(vec.vec())) return;

	for (Entity& e : entities)
		e.move(vec);

	for (auto& c : connections)
		c.move(vec);
}

void Part::moveTo(const Vertex& pt)
{
	using namespace std;
	using namespace placeholders;

	auto it = find_if(entities.begin(), entities.end(), &Entity::entworldcmp);
	Vertex wOrig;

	if (it == entities.end())
		wOrig = Vertex(0, 0, 0);
	else
		wOrig = it->origin();

	Vector mov = Vector::diff(wOrig, pt);
	move(mov);
}

void Part::rotate(const Angle& angle, const Vertex& pt)
{
	auto it = find_if(entities.begin(), entities.end(), &Entity::entworldcmp);

	Vertex orig;
	if (Vertex::isVertex(pt))
		orig = pt;
	else if (it == entities.end())
		orig = Vertex(0, 0, 0);
	else
		orig = it->origin();

	Angle rotangle(-angle.pitch(), angle.yaw(), angle.roll()); //Invert pitch for compliance with hammer.
	auto rotmat = rotangle.angleMatrix();

	for (Entity& e : entities)
	{
		e.rotate(rotmat, orig);
	}

	for (Connection& c : connections)
	{
		c.rotate(rotmat, orig);
	}
}

std::streampos Part::toFile(std::string filename) const
{
	std::ofstream file(filename, std::ios::trunc);
	file << *this;
	std::streampos fsize = file.tellp();
	file.close();
	return fsize;
}

void Part::reID()
{
	entityID_ = 0;
	solidID_ = 0;
	sideID_ = 0;
	for (Entity& e : entities)
	{
		e.reID(entityID_, solidID_, sideID_);
	}
}

//Merges world entities and copies all other entities
Part& Part::operator+=(const Part& rhs)
{
	//Entity *tWorld = entities.get_first_match<std::string>
	//	("worldspawn", Entity::entclasscmp),
	//	*origWorld = rhs.entities.get_first_match<std::string>
	//	("worldspawn", Entity::entclasscmp);
	auto cit = find_if(entities.begin(), entities.end(), &Entity::entworldcmp);
	auto origit = find_if(rhs.entities.cbegin(), rhs.entities.cend(), &Entity::entworldcmp);
	if (cit != entities.end() && origit != rhs.entities.cend())
	{
		cit->mergeSolids(*origit);
	}
	else if (cit == entities.end() && origit != rhs.entities.cend())
	{
		entities.push_back(*origit);
	}
	size_t cap = std::numeric_limits<unsigned int>::max(), count = 0;
	if (this == &rhs) cap = entities.size(); //Safety against self-addition.
	for (const Entity& entity : rhs.entities)
	{
		if (count++ == cap) break;
		if (entity["classname"] == "worldspawn") continue;
		entities.push_back(entity);
	}
	reID(); // Faster to call this here than make lots of copies everywhere.
	return *this;
}

//Returns reference to a entity with classname equal to argument.
//If no such entity exists, the function will create one and return its reference.
Entity& Part::operator[](std::string classname)
{
	auto it = find_if(entities.begin(), entities.end(), bind(&Entity::entclasscmp, std::placeholders::_1, classname));
	if (it == entities.end())
	{
		it = entities.emplace(entities.end(), classname);
	}
	return *it;
}

Part::Part(void)
{
	entityID_ = 0;
	solidID_ = 0;
	sideID_ = 0;
}

Part::Part(std::string filepath) : Part()
{
	parse(filepath);
}


Part::~Part(void)
{
}

