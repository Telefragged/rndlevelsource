#include "stdafx.h"
#include <sstream>
#include "Angle.h"
#include "BoundingBox.h"
#include "Entity.h"
#include "Matrix.h"
#include "Vector.h"
#include "Vertex.h"

unsigned int Entity::parse(std::istream& stream)
{
	unsigned int numparsed = 0;
	std::string curline;
	while (trim(curline) != "{")
	{
		std::getline(stream, curline);
		numparsed++;
	}
	unsigned int depth = 1;
	while (std::getline(stream, curline))
	{
		numparsed++;
		if (trim(curline) == "solid")
		{
			auto it = solids.emplace(solids.end());
			numparsed += it->parse(stream);
		}
		else if (trim(curline) == "editor")
		{
			numparsed += edt.parse(stream);
		}
		else if (trim(curline) == "group")
		{
			Solid solid;
			numparsed += solid.parse(stream);
		}
		else if (trim(curline) == "}")
		{
			if (--depth == 0) break;
		}
		else if (trim(curline) == "{")
		{
			++depth;
		}
		else
		{
			KeyVal parsed(curline);
			keyvals[parsed.key] = parsed.val;
		}
	}
	if (keyvals.count("id") > 0)
	{
		this->id_ = atoi(keyvals["id"].c_str());
		keyvals.erase("id");
	}
	return numparsed;
}

bool Entity::testCollision(const Entity& lhs, const Entity& rhs)
{
	if (BoundingBox::testCollision(lhs.bbox(), rhs.bbox()))
	{
		for (const Solid& lhsolid : lhs.solids)
		{
			for (const Solid& rhsolid : rhs.solids)
			{
				if (Solid::testCollision(lhsolid, rhsolid)) return true;
			}
		}
	}
	return false;
}

bool Entity::entclasscmp(const Entity& lhs, const std::string& rhs)
{
	if (lhs.keyvals.count("classname") == 0)
		return false;
	else if (lhs.keyvals.at("classname") == rhs)
		return true;
	else
		return false;
}

bool Entity::entworldcmp(const Entity& entity)
{
	return entity["classname"] == "worldspawn";
}

Entity Entity::defaultWorldEntity()
{
	Entity world;
	world.id_ = 1;
	world.keyvals["classname"] = "worldspawn";
	world.keyvals["mapversion"] = "1";
	world.keyvals["detailmaterial"] = "detail/detailsprites";
	world.keyvals["detailvbsp"] = "detail.vbsp";
	world.keyvals["maxpropscreenwidth"] = "-1";
	world.keyvals["skyname"] = "sky_day_01_01";
	//world.keyvals.put(new KeyVal("classname", "world"));
	//world.keyvals.put(new KeyVal("mapversion", "1"));
	//world.keyvals.put(new KeyVal("classname", "worldspawn"));
	//world.keyvals.put(new KeyVal("detailmaterial", "detail/detailsprites"));
	//world.keyvals.put(new KeyVal("detailvbsp", "detail.vbsp"));
	//world.keyvals.put(new KeyVal("maxpropscreenwidth", "-1"));
	//world.keyvals.put(new KeyVal("skyname", "sky_day01_01"));
	return world;
}

Vertex Entity::origin() const
{
	auto val = (*this)["origin"];
	if (val != "") return Vertex(val);
	BoundingBox b = bbox();
	Vector vec(b.min, b.max);
	Vertex mod = vec.vec() * 0.5;
	return vec.beg() + mod;
}

Vertex Entity::originKV() const
{
	if (keyvals.count("origin") > 0) return Vertex(keyvals.at("origin"));
	return Vertex();
}

Angle Entity::angles() const
{
	auto val = (*this)["angles"];
	if (val != "") return Angle(val);
	return Angle();
}

void Entity::mergeSolids(const Entity& entity)
{
	solids.reserve(solids.size() + entity.solids.size());
	solids.insert(solids.end(), entity.solids.begin(), entity.solids.end());
}

void Entity::rotate(const Matrix& rotmat, const Vertex& pt)
{
	for (Solid& s : solids)
	{
		s.rotate(pt, rotmat);
	}
	Angle thAngle = angles();
	if (Angle::isAngle(thAngle))
	{
		thAngle.fromMatrix(rotmat * thAngle.angleMatrix());
		(*this)["angles"] = thAngle.toStr();
	}
	Vertex orig = originKV();
	if (Vertex::isVertex(orig))
	{
		orig = orig.rotate(pt, rotmat);
		(*this)["origin"] = orig.toStr();
	}
}

void Entity::move(const Vector& vec)
{
	for (Solid& s : solids)
	{
		s.move(vec);
	}
	if (keyvals.count("origin") > 0)
	{
		std::string origStr = keyvals.at("origin");
		Vertex orig(origStr);
		orig += vec.vec();
		keyvals["origin"] = orig.toStr();
	}
}

void Entity::reID(unsigned int* entityID,
                  unsigned int* solidID,
                  unsigned int* sideID)
{
	id_ = ++(*entityID);
	for (Solid& solid : solids)
	{
		solid.reID(solidID, sideID);
	}
}

BoundingBox Entity::bbox() const
{
	const double dmax = std::numeric_limits<double>::max();
	const double dmin = -dmax;
	Vertex min(dmax, dmax, dmax);
	Vertex max(dmin, dmin, dmin);
	for (const Solid& s : solids)
	{
		BoundingBox sbox = s.bbox();
		min = Vertex::allmin(min, sbox.min);
		max = Vertex::allmax(max, sbox.max);
	}
	return BoundingBox(min, max);
}

Entity::Entity(void)
{
}

Entity::Entity(const std::string& classname) :
	id_(0)
{
	keyvals["classname"] = classname;
}

Entity::~Entity(void)
{
}

