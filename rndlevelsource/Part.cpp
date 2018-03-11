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

size_t Part::parse(std::istream& stream)
{
	size_t numparsed = 0;
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

size_t Part::parse(std::string_view path)
{
	std::string p(path);

	std::ifstream file(p);
	if (!file.good())
		throw std::exception("Failed to open file");

	auto ret = parse(file);
	file.close();
	return ret;
}

unsigned int Part::countEntities(const std::string& classname) const
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

Vertex Part::origin() const
{
	auto it = find_if(entities.begin(), entities.end(), &Entity::entworldcmp);

	if (it == entities.end())
		return { 0, 0, 0 };

	return it->origin();
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

void Part::moveTo(const Vertex& point)
{

	auto it = std::find_if(entities.begin(), entities.end(), &Entity::entworldcmp);
	Vertex wOrig;

	if (it == entities.end())
		wOrig = Vertex(0, 0, 0);
	else
		wOrig = it->origin();

	Vector mov = Vector::diff(wOrig, point);
	move(mov);
}

void Part::rotate(const Angle& angle, const Vertex& point)
{
	auto it = find_if(entities.begin(), entities.end(), &Entity::entworldcmp);

	Vertex orig;
	if (Vertex::isVertex(point))
		orig = point;
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

void Part::scale(const Vertex & scale, const Vertex& origin, bool ignoreEntities)
{
	auto it = find_if(entities.begin(), entities.end(), &Entity::entworldcmp);

	if (it != entities.end())
	{
		for (Solid &s : it->solids)
			s.scale(scale, origin);
	}

	if (!ignoreEntities)
	{
		for (Entity &e : entities)
		{
			if (e["classname"] == "worldspawn")
				continue;

			Vertex entityOrigin = e.originKV();

			if (Vertex::isVertex(entityOrigin))
			{
				Vertex vec = Vector::diff(origin, entityOrigin).vec();
				vec.x(vec.x() * scale.x());
				vec.y(vec.y() * scale.y());
				vec.z(vec.z() * scale.z());

				e["origin"] = (origin + vec).toStr();
			}
			for (Solid &s : e.solids)
				s.scale(scale, origin);
		}
	}

	for (Connection &c : connections)
	{
		Vertex vec = Vector::diff(origin, c.origin()).vec();
		vec.x(vec.x() * scale.x());
		vec.y(vec.y() * scale.y());
		vec.z(vec.z() * scale.z());

		c["origin"] = (origin + vec).toStr();
	}
}

void Part::scale(const Vertex& scale, bool ignoreEntities)
{
	auto it = find_if(entities.begin(), entities.end(), &Entity::entworldcmp);

	Vertex orig = this->origin();
	if (it != entities.end())
	{
		for (Solid &s : it->solids)
			s.scale(scale, orig);
	}

	this->scale(scale, orig, ignoreEntities);
}

void Part::scaleTo(double length)
{
	if (connections.size() != 2)
		throw std::exception("Part needs exactly 2 connections for scaling to work");

	if (Vertex::countDifferentAxes(connections[1].origin(), connections[0].origin()) > 1)
		throw std::exception("Connections can only have one differing axis");

	Vertex diff = connections[1].origin() - connections[0].origin();

	Vertex scale = Vertex::absolute(diff);

	size_t differentAxis = 0;

	if (!doubleeq(scale.x(), 0))
	{
		scale.x(length / scale.x());
		differentAxis = 0;
	}
	else
	{
		scale.x(1.0);
	}

	if (!doubleeq(scale.y(), 0))
	{
		scale.y(length / scale.y());
		differentAxis = 1;
	}
	else
	{
		scale.y(1.0);
	}

	if (!doubleeq(scale.z(), 0))
	{
		scale.z(length / scale.z());
		differentAxis = 2;
	}
	else
	{
		scale.z(1.0);
	}

	Vertex orig = this->origin();

	//Set origin at one end of the room so we "stretch" the room towards the other side.
	orig[differentAxis] = connections[0].origin()[differentAxis];

	this->scale(scale, orig, true);

	size_t numEntities = entities.size();

	for (size_t i = 0; i < numEntities; i++)
	{
		if (entities[i]["classname"] == "worldspawn")
			continue;

		Vertex entityOrig = entities[i].originKV();

		if (!Vertex::isVertex(entityOrig))
			continue;

		// Get offset to closest connection
		double offset = entityOrig[differentAxis] - connections[0].origin()[differentAxis];

		if (std::fabs(offset) >
			std::fabs(entityOrig[differentAxis] - connections[1].origin()[differentAxis]))
		{
			offset = entityOrig[differentAxis] - connections[1].origin()[differentAxis];
		}

		double repeat = diff[differentAxis];

		for (size_t mult = 1; std::fabs(repeat) * mult + std::fabs(offset) < length; mult++)
		{
			Entity copy = entities[i];

			Vertex copyOrig = entityOrig;

			copyOrig[differentAxis] += repeat * mult;

			copy["origin"] = copyOrig.toStr();

			entities.push_back(copy);
		}
	}
	reID();
}

std::streampos Part::toFile(const std::string& path) const
{
	std::ofstream file(path, std::ios::trunc);
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
		e.reID(entityID_, solidID_, sideID_);
}

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

	size_t cap = std::numeric_limits<size_t>::max(), count = 0;

	if (this == &rhs)
		cap = entities.size(); //Safety against self-addition.

	for (const Entity& entity : rhs.entities)
	{
		if (count++ == cap)
			break;

		if (entity["classname"] == "worldspawn")
			continue;

		entities.push_back(entity);
	}
	reID(); // Faster to call this here than make lots of copies everywhere.
	return *this;
}

Entity& Part::operator[](const std::string& classname)
{
	auto it = find_if(entities.begin(), entities.end(), bind(&Entity::entclasscmp, std::placeholders::_1, classname));
	if (it == entities.end())
	{
		it = entities.emplace(entities.end(), classname);
	}
	return *it;
}

Part::Part(std::string_view path) : Part()
{
	parse(path);
}
