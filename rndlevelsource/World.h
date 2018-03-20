#pragma once
#include <vector>
#include <random>

#include "Part.h"

class World
{
	std::vector<Part> master;
	std::mt19937_64 eng_;

public:

	World();
	~World();
};

