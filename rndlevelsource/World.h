#pragma once
#include <vector>
#include <random>
#include <memory>

#include "WeightedVector.h"

class Part;
class Connection;

class World
{
	WeightedVector<std::shared_ptr<Part>> starts, scaleables, inters;
	std::mt19937_64 eng_;

	std::vector<Part> world_;

public:
	bool addPart(Part&& part);
	size_t testCollision(const Part& part, Part *ignore = nullptr) const;

	void buildInitial(size_t length);

	Part serialize() const;

	World();
	~World() = default;
};
