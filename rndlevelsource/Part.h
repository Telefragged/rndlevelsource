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
	static bool testCollision(const Part&, const Part&);

	// Generate a random room
	// Thickness determines the brush thickness
	static Part createRoom(std::mt19937& eng, const Vector& pos, double thickness);

	// Parse contents of stream in .vmf format
	// Parses until end of stream
	unsigned int parse(std::istream&);
	// Open and parse file in .vmf format.
	unsigned int parse(std::string);
	// Counts number of entities in the part with the exact same classname
	unsigned int countEntities(std::string) const;

	// Opens and writes a file in .vmf format
	std::streampos toFile(std::string) const;

	// Returns an AABB with the min and max coordinates of this part
	BoundingBox bbox() const;

	// Move the part
	void move(const Vector&);

	// Move the part to specified coordinates
	void moveTo(const Vertex&);

	// Performs a rotation around a fixed point
	// By default the part rotates around the centre
	// of its AABB
	void rotate(const Angle&, const Vertex& pt = Vertex());

	// Re-assigns all entity-, solid- and sideids so that they are unique and in order
	// NOTE -- this is called by the parse and += methods
	void reID();

	Part& operator+=(const Part&);
	Entity& operator[](std::string);

	friend std::ostream& operator<<(std::ostream&, const Part&);
	friend std::istream& operator>>(std::istream&, Part&);

	Part();

	// Open and parse file in .vmf format
	Part(std::string);
	~Part();
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

