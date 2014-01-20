#pragma once
#include "Part.h"
#include "LinkedList.h"
#include "WeightedList.h"

class World
{
	LinkedList<Part> parts;
	WeightedList<Part> master;
	std::mt19937 eng_;
public:

	bool testCollisions(Part *);

	void addMaster(const Part &);
	void addPart(const Part &);
	void movePart(Part *, Connection *);

	Part collapse();

	Connection *randConnection(Part &);

	World(void);
	~World(void);
};

