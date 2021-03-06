#pragma once
#include <ios>
#include <random>

#include "Connection.h"
#include "WeightedVector.h"
#include "PartInfo.h"

class Entity;
class BoundingBox;
class DispInfo;

class Part
{
private:
	unsigned int entityID_, solidID_, sideID_;
	using ConnectionIteratorType = WeightedVector<Connection>::iterator;
public:
	PartInfo info;

	std::vector<Entity> entities;
	std::string filename;
	WeightedVector<Connection> connections;

	//Test collision between parts
	static bool testCollision(const Part& lhs, const Part& rhs);

	// Generate a random room
	// Thickness determines the brush thickness
	static Part createRoom(std::mt19937& eng, const Vector& pos, double thickness);

	static bool startPredicate(const Part& part);
	static bool scaleablePredicate(const Part& part);
	static bool interPredicate(const Part& part);

	// Parse contents of stream in .vmf format
	// Parses until end of stream
	size_t parse(std::istream& stream);
	// Open and parse file in .vmf (valve map format) format.
	size_t parse(std::string_view path);
	// Counts number of entities in the part with the exact same classname
	size_t countEntities(const std::string& classname) const;

	// Opens and writes a file in .vmf format
	std::streampos toFile(const std::string& path) const;

	// Returns an AABB with the min and max coordinates of this part
	BoundingBox bbox() const;

	Vertex origin() const;

	// Move the part
	void move(const Vector& vec);

	// Move the part to specified coordinates
	void moveTo(const Vertex& point);

	// Performs a rotation around a fixed point
	// By default the part rotates around its origin
	void rotate(const Angle& angle, const Vertex& point = Vertex());

	void rotate(const Matrix3d& rotmat, const Vertex& point = Vertex());

	// Scales the part around the given origin
	void scale(const Vertex& scale, const Vertex& origin, bool ignoreEntities = false);

	// Scales the part around its origin
	void scale(const Vertex& scale, bool ignoreEntities = false);

	// Scales the part to desired length
	// Needs exactly two connections that share exactly two axes
	void scaleTo(double length);

	// Re-assigns all entity-, solid- and sideids so that they are unique and in order
	// NOTE -- this is called by the parse and += methods
	void reID();

	//Selects a random connection from this part
	template <class Eng>
	ConnectionIteratorType selectRandomConnection(Eng &engine)
	{
		if (connections.totalWeight() == 0)
			return connections.end();

		std::uniform_int_distribution<ptrdiff_t> dist(0, connections.totalWeight() - 1);

		auto ret = connections.getWeightedIter(dist(engine));
		ret->parent = this;

		return ret;
	}

	//Merges world entities and copies all other entities
	Part& operator+=(const Part& rhs);

	Part& operator+=(Part&& rhs);

	Part operator+(const Part& rhs) const;

	//Returns reference to a entity with classname equal to argument.
	//If no such entity exists, the function will create one and return its reference.
	Entity& operator[](const std::string& classname);

	friend std::ostream& operator<<(std::ostream& os, const Part& p);
	friend std::istream& operator>>(std::istream& is, Part& p);

	Part() = default;

	// Open and parse file in .vmf format
	Part(std::string_view path);
	~Part() = default;
};

inline std::ostream& operator<<(std::ostream& os, const Part& p)
{
	char prevfill = os.fill('\t');
	for (const Entity& entity : p.entities)
	{
		entity.depth(0);
		os << entity << "\n";
	}
	os.fill(prevfill);
	return os;
}

inline std::istream& operator>>(std::istream& is, Part& p)
{
	p.parse(is);
	return is;
}

