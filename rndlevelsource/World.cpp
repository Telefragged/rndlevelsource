#include "World.h"

#include <boost/range/algorithm.hpp>

#include "Part.h"

bool World::addPart(Part&& part)
{
	if (Part::scaleablePredicate(part))
	{
		scaleables.push_back(std::move(part));
		return true;
	}

	if (Part::startPredicate(part))
	{
		starts.push_back(std::move(part));
		return true;
	}

	if (Part::interPredicate(part))
	{
		inters.push_back(std::move(part));
		return true;
	}

	return false;
}

void World::buildInitial()
{
}

World::World()
{
	std::random_device dev;
	eng_.seed(dev());
}
