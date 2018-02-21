#pragma once
#include <ios>
#include <random>
#include "Connection.h"
#include "Entity.h"
#include "WeightedVector.h"

class BoundingBox;
class DispInfo;

class Part
{
private:
	unsigned int entityID_, solidID_, sideID_;
public:
	std::vector<Entity> entities;
	WeightedVector<Connection> connections;

	//Test collision between parts
	static bool testCollision(const Part& lhs, const Part& rhs);

	// Generate a random room
	// Thickness determines the brush thickness
	static Part createRoom(std::mt19937& eng, const Vector& pos, double thickness);

	// Parse contents of stream in .vmf format
	// Parses until end of stream
	unsigned int parse(std::istream& stream);
	// Open and parse file in .vmf (valve map format) format.
	unsigned int parse(const std::string& path);
	// Counts number of entities in the part with the exact same classname
	unsigned int countEntities(const std::string& classname) const;

	// Opens and writes a file in .vmf format
	std::streampos toFile(const std::string& path) const;

	// Returns an AABB with the min and max coordinates of this part
	BoundingBox bbox() const;

	// Move the part
	void move(const Vector& vec);

	// Move the part to specified coordinates
	void moveTo(const Vertex& point);

	// Performs a rotation around a fixed point
	// By default the part rotates around the centre
	// of its AABB
	void rotate(const Angle& angle, const Vertex& point = Vertex());

	// Re-assigns all entity-, solid- and sideids so that they are unique and in order
	// NOTE -- this is called by the parse and += methods
	void reID();

	Part& operator+=(const Part& rhs);
	Entity& operator[](const std::string& classname);

	friend std::ostream& operator<<(std::ostream& os, const Part& p);
	friend std::istream& operator>>(std::istream& is, Part& p);

	Part() = default;

	// Open and parse file in .vmf format
	Part(std::string);
	~Part() = default;
};

inline std::ostream& operator<<(std::ostream& os, const Part& p)
{
	char prevfill = os.fill();
	os.fill('\t');
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

