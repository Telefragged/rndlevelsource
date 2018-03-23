#include "Entity.h"

#include <boost/range/algorithm.hpp>

#include "Angle.h"
#include "BoundingBox.h"
#include "Matrix.h"
#include "Vector.h"
#include "Vertex.h"

size_t Entity::parseSpecial(std::istream& stream, std::string_view type)
{
	if (type == "solid")
	{
		auto it = solids.emplace(solids.end());
		auto ret = it->parse(stream);
		it->fixSides();
		return ret;
	}

	if (type == "editor")
	{
		return edt.parse(stream);
	}

	if (type == "group")
	{
		Solid solid;
		return solid.parse(stream);
	}

	return 0;
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
	if (auto it = lhs.keyvals.find("classname");  it->second == rhs)
		return true;

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

	if (!val.empty())
		return Vertex(val);

	BoundingBox b = bbox();
	Vector vec = Vector::diff(b.min, b.max);
	Vertex mod = 0.5 * vec.vec();
	return vec.beg() + mod;
}

Vertex Entity::originKV() const
{
	if (keyvals.count("origin") > 0)
		return Vertex(keyvals.at("origin"));

	return Vertex();
}

Angle Entity::angles() const
{
	auto val = (*this)["angles"];

	if (!val.empty())
		return Angle(val);

	return Angle();
}

void Entity::mergeSolids(const Entity& entity)
{
	boost::copy(entity.solids, std::back_inserter(solids));
}

void Entity::mergeSolids(Entity&& entity)
{

	std::move(entity.solids.begin(), entity.solids.end(), std::back_inserter(solids));
}

void Entity::rotate(const Matrix3d& rotmat, const Vertex& pt)
{
	for (Solid& s : solids)
	{
		s.rotate(pt, rotmat);
	}

	Angle angle = angles();
	if (Angle::isAngle(angle))
	{
		angle.fromMatrix(rotmat * angle.angleMatrix());
		(*this)["angles"] = angle.toStr();
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

void Entity::reID(unsigned int &entityID,
                  unsigned int &solidID,
                  unsigned int &sideID)
{
	id_ = ++entityID;
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

void Entity::extraOutput(std::ostream & os) const
{
	for (const Solid& solid : solids)
	{
		solid.depth(depth() + TABDEPTH);
		os << solid << "\n";
	}
	edt.depth(depth() + TABDEPTH);
	os << edt << "\n";
}

std::string Entity::getName() const
{
	if ((*this)["classname"] == "worldspawn")
	{
		return "world";
	}

	return "entity";
}

Entity::Entity(const std::string& classname)
{
	keyvals["classname"] = classname;
}
