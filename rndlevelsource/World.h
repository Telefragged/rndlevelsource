#pragma once
#include <vector>
#include <random>

#include "WeightedVector.h"

class Part;

class World
{
	WeightedVector<Part> starts, scaleables, inters;
	std::mt19937_64 eng_;

	std::vector<Part> world;

public:
	bool addPart(Part&& part);

	void buildInitial();

	World();
	~World() = default;
};

