#pragma once
#include <vector>
#include "Part.h"

using parttuple = std::tuple<Part, unsigned int, unsigned int, const Connection *, const Connection *>;

class World
{
	std::vector<Part> master;
	std::mt19937_64 eng_;

	static void filterFitHelper(const Part& p1, const Part& p2, const Part& test, std::vector<parttuple>& resList);

public:

	std::vector<Part> parts;

	bool testCollisions(Part* partptr);

	void addMaster(const Part& part);

	template <typename _Ty, typename... Args>
	void addMaster(_Ty t, Args ... args)
	{
		addMaster(t);
		addMaster(args...);
	}

	void addPart(const Part& part);

	void buildWorld();
	// Align the part at a random connection to the selected connection
	void movePart(Part* part, const Connection* prevc);
	void removePart(size_t index);

	static void movePart(Part* part, Connection* newc, const Connection* prevc);

	Part collapse();

	Connection* randConnection(Part& part);

	Part* getPart(size_t index);

	// Copies all combinations of all parts from the candidate list that fit between the two parts
	static std::vector<parttuple> filterFit(const Part& p1, const Part& p2, const std::vector<Part>& cands);
	std::vector<Part> filter(const Connection* connection, const std::vector<Part>& parts);

	World();
	~World();
};

