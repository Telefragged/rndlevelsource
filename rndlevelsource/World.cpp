#include "World.h"

#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>

#include "Part.h"
#include "BoundingBox.h"
#include "Timer.h"

bool World::addPart(Part&& part)
{
	auto ptr = std::make_shared<Part>(std::move(part));

	bool ret = false;

	if (Part::scaleablePredicate(part))
	{
		scaleables.push_back(ptr);
		ret = true;
	}

	if (Part::startPredicate(part))
	{
		starts.push_back(ptr);
		ret = true;
	}

	if (Part::interPredicate(part))
	{
		inters.push_back(ptr);
		ret = true;
	}

	return ret;
}

size_t World::testCollision(const Part& part, Part *ignore) const
{
	nanotimer timer(" miliseconds", 1'000'000.0);

	for (const auto& otherPart : world_ | boost::adaptors::indexed(1))
	{
		if (&otherPart.value() == ignore || &otherPart.value() == &part)
			continue;

		if (Part::testCollision(part, otherPart.value()))
			return otherPart.index();
	}

	return 0;
}

void World::buildInitial(size_t length)
{
	auto &startPart = world_.emplace_back(**starts.getRandomWeightedIter(eng_));

	auto box = startPart.bbox();

	static size_t numCollisions = 0;

	startPart.moveTo((box.max - box.min) / 2);

	size_t openConnections = 0;

	for (size_t n = 0; n < length; n++)
	{
		std::uniform_int_distribution<ptrdiff_t> interDist(0, inters.totalWeight() - 1);

		auto weightedPart = *inters.getWeightedAndRedistribute(interDist(eng_), eng_);

		auto &newPart = world_.emplace_back(*weightedPart);

		auto &prevPart = world_[world_.size() - 2];
		auto prevc = prevPart.selectRandomConnection(eng_);
		auto newc = newPart.selectRandomConnection(eng_);

		newc->connectTo(&*prevc);

		if (size_t collisionIndex = testCollision(newPart, nullptr); collisionIndex > 0)
		{
			std::string filename = "F:\\test\\Collisions\\Collision " + std::to_string(++numCollisions) + ".vmf";
			std::cout << "Warning: Collision room " << n + 2 << ", " << collisionIndex << ", writing file " << filename << "\n";
			Part collisionPart = newPart + world_[collisionIndex - 1];
			collisionPart.toFile(filename);
		}

		newPart.connections.setWeight(newc, 0);
		prevPart.connections.setWeight(prevc, 0);

		openConnections += prevPart.connections.reachable();
	}
}

Part World::serialize() const
{
	Part ret;
	for (const auto& part : world_)
		ret += part;
	return ret;
}

World::World()
{
	std::random_device dev;
	eng_.seed(dev());
}
