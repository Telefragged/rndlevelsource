#pragma once
#include <vector>
#include "Part.h"

typedef std::tuple<Part, unsigned int, unsigned int, const Connection *, const Connection *> parttuple;

class World
{
	std::vector<Part> master;
	std::mt19937 eng_;

	static void World::filterFitHelper(const Part &p1, const Part &p2, const Part &test, std::vector<parttuple> &resList);

public:

	std::vector<Part> parts;

	bool testCollisions(Part *);

	void addMaster(const Part &);

	template <typename _Ty, typename... Args>
	void addMaster(_Ty t, Args... args) {
		addMaster(t);
		addMaster(args...);
	}

	void addPart(const Part &);

	void buildWorld();
	// Align the part at a random connection to the selected connection
	void movePart(Part *, const Connection *);
	void removePart(unsigned int);

	static void movePart(Part *, Connection *, const Connection *);

	Part collapse();

	Connection *randConnection(Part &);

	Part *getPart(unsigned int);

	// Copies all combinations of all parts from the candidate list that fit between the two parts
	static std::vector<parttuple> filterFit(const Part&, const Part&, const std::vector<Part> &);
	std::vector<Part> filter(const Connection *, const std::vector<Part> &);

	World(void);
	~World(void);
};
