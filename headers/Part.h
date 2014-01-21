#pragma once
#include <ios>
#include "Connection.h"
#include "Entity.h"
#include "LinkedList.h"
#include "WeightedList.h"

class BoundingBox;

class Part
{
private:
	unsigned int entityID_, solidID_, sideID_;
public:
	LinkedList<Entity> entities;
	WeightedList<Connection> connections;

	//Test collision between parts
	static bool testCollision(const Part &, const Part &);

	//Parse contents of stream in .vmf format.
	//Parses until end of stream.
	unsigned int parse(std::istream &);

	//Open and parse file in .vmf format.
	unsigned int parse(std::string);

	//Opens and writes a file in .vmf format.
	std::streampos toFile(std::string);

	//Returns an AABB with the min and max coordinates of this part.
	BoundingBox bbox() const;

	//Move the part by the internal vector.
	void move(const Vector &);

	//Move the part to specified coordinates.
	void moveTo(const Vertex &);

	//Performs a rotation around a fixed point.
	//By default the part rotates around the centre
	//of its AABB.
	void rotate(const Angle &, const Vertex &pt = Vertex());

	//Re-assigns all entity-, solid- and sideids so that they are unique and in order.
	//NOTE -- this is called by the parse and += methods.
	void reID();

	Part &operator=(Part &&);
	Part &operator+=(const Part &);
	Entity &operator[](std::string);

	friend std::ostream &operator<<(std::ostream &, const Part &);
	friend std::istream &operator>>(std::istream &, Part &);

	Part(void);

	//Open and parse file in .vmf format.
	Part(std::string);
	Part(const Part &);
	Part(Part &&);
	~Part(void);
};

inline std::ostream &operator<<(std::ostream &os, const Part &p) {
	char prevfill = os.fill();
	os.fill('\t');
	for(const Entity &entity : p.entities) {
		entity.depth(0);
		os<<entity<<"\n";
	}
	os.fill(prevfill);
	return os;
}

inline std::istream &operator>>(std::istream &is, Part &p) {
	p.parse(is);
	return is;
}
