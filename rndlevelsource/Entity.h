#pragma once
#include "Solid.h"
#include "KeyValBase.h"
#include "Editor.h"

class Vertex;
class Angle;
class Connection;

class Entity :
	public KeyValBase
{
private:
	Editor edt;
public:
	unsigned int id_;
	std::vector<Solid> solids;

	Vertex origin() const;
	Vertex originKV() const;
	Angle angles() const;

	unsigned int parse(std::istream& stream);

	static bool testCollision(const Entity& lhs, const Entity& rhs);
	static bool entclasscmp(const Entity& lhs, const std::string& rhs);
	static bool entworldcmp(const Entity& entity);

	// Creates a default "worldspawn" entity.
	static Entity defaultWorldEntity();

	void rotate(const Matrix3d& rotmat, const Vertex& pt);
	void move(const Vector& vec);
	void mergeSolids(const Entity& entity);
	void reID(unsigned int& entityID, unsigned int& solidID, unsigned int& sideID);

	BoundingBox bbox() const;

	friend std::ostream& operator<<(std::ostream& os, const Entity& e);

	friend class Connection;

	Entity() = default;
	Entity(const std::string& classname);
	~Entity() = default;
};

inline std::ostream& operator<<(std::ostream& os, const Entity& e)
{
	if (e["classname"] == "worldspawn")
	{
		os << std::setw(e.depth()) << "" << "world\n";
	}
	else
	{
		os << std::setw(e.depth()) << "" << "entity\n";
	}
	os << std::setw(e.depth()) << "" << "{\n";
	os << std::setw(e.depth()) << "" << "\t\"id\" \"" << e.id_ << "\"\n";
	for (const auto& pair : e.keyvals)
	{
		//if (pair.first == "classname" && pair.second == "worldspawn") continue;
		os << std::setw(e.depth()) << "\t" << KeyValBase::toStr(pair) << "\n";
	}
	for (const Solid& solid : e.solids)
	{
		solid.depth(e.depth() + TABDEPTH);
		os << solid << "\n";
	}
	e.edt.depth(e.depth() + TABDEPTH);
	os << std::setw(e.depth()) << "" << "}";
	return os;
}

