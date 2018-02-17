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

	unsigned int parse(std::istream&);

	static bool testCollision(const Entity&, const Entity&);
	static bool entclasscmp(const Entity&, const std::string&);
	static bool entworldcmp(const Entity&);

	static Entity defaultWorldEntity();

	void rotate(const Matrix3d&, const Vertex&);
	void move(const Vector&);
	void mergeSolids(const Entity&);
	void reID(unsigned int&, unsigned int&, unsigned int&);

	BoundingBox bbox() const;

	friend std::ostream& operator<<(std::ostream&, const Entity&);

	friend Connection;

	Entity();
	Entity(const std::string&);
	~Entity();
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

