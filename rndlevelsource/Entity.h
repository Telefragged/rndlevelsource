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

	void extraOutput(std::ostream& os) const override;
	std::string getName() const override;

	friend class Connection;

	Entity() = default;
	Entity(const std::string& classname);
	~Entity() = default;
};

