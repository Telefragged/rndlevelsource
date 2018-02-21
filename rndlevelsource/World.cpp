#include "World.h"

#include <algorithm>
#include <chrono>

#include "Angle.h"
#include "BoundingBox.h"
#include "Matrix.h"
#include "Vector.h"

bool World::testCollisions(Part* partptr)
{
	if (partptr == nullptr) return true;
	size_t npartn = parts.size(), partn = 0;
	for (const Part& p : parts)
	{
		if (Part::testCollision(p, *partptr))
		{
			printf("Collision room %zu and %zu\n", partn, npartn);
			printf("[(%s) (%s)] %zu collision [(%s) (%s)] %zu\n\n",
			       p.bbox().min.toStr().c_str(), p.bbox().max.toStr().c_str(), partn,
			       partptr->bbox().min.toStr().c_str(), partptr->bbox().max.toStr().c_str(), npartn);
			return true;
		}
		partn++;
	}
	return false;
}

void World::addMaster(const Part& part)
{
	master.push_back(part);
}

void World::addPart(const Part& part)
{
	if (parts.size() == 0)
	{
		parts.emplace_back(part);
		return;
	}
	Part* prevpart = &parts.back();
	Connection* prevc = randConnection(*prevpart);
	if (prevc == nullptr)
	{
		printf("Failed to add part\n");
		return;
	}
	Part newpart(part);
	movePart(&newpart, prevc);
	testCollisions(&newpart);
	parts.push_back(std::move(newpart));
}

void World::movePart(Part* part, const Connection* prevc)
{
	Connection* newc = randConnection(*part);
	movePart(part, newc, prevc);
}

void World::movePart(Part* part, Connection* newc, const Connection* prevc)
{
	Angle targetAngle = prevc->angles();
	targetAngle[YAW] += 180.0;
	Angle newAngle = newc->angles();

	Angle rotAngle;
	rotAngle.fromMatrix(newAngle.calcRotation(targetAngle));

	part->rotate(rotAngle);
	Vector mov = Vector::diff(newc->origin(), prevc->origin());
	part->move(mov);
}

Part World::collapse()
{
	Part ret;
	for (Part& part : parts)
	{
		ret += part;
	}
	return ret;
}

Connection* World::randConnection(Part& part)
{
	if (part.connections.weight() == 0) return nullptr;
	std::uniform_int_distribution<size_t> dist(0, part.connections.weight() - 1);
	auto ptr = part.connections.getWeighted(dist(eng_));
	part.connections.setWeight(ptr, 0);
	return ptr;
}

void World::removePart(size_t index)
{
	if (index >= parts.size()) return;
	parts.erase(parts.begin() + index);
}

Part* World::getPart(size_t index)
{
	if (index >= parts.size()) return nullptr;
	return &parts.at(index);
}

void World::buildWorld()
{
	std::vector<Part> startParts(master.size()), interParts(master.size()), endParts(master.size());
	auto startIter = std::copy_if(master.cbegin(), master.cend(), startParts.begin(), [](const Part& part)
	                {
		                //startParts test
		                return (part.countEntities("info_player_start") > 0) && (part.connections.size() >= 1);
	                });

	auto interIter = std::copy_if(master.cbegin(), master.cend(), interParts.begin(), [](const Part& part)
					{
						//interParts test
						return (part.countEntities("info_player_start") == 0) && (part.connections.size() >= 2);
					});

	auto endIter = std::copy_if(master.cbegin(), master.cend(), endParts.begin(), [](const Part& part)
					{
						//endParts test
						return (part.countEntities("info_player_start") == 0) && (part.connections.size() == 1);
					});

	startParts.resize(distance(startParts.begin(), startIter));
	interParts.resize(distance(interParts.begin(), interIter));
	endParts.resize(distance(endParts.begin(), endIter));

	WeightedVector<Part> weightedInter(interParts, 5);

	std::uniform_int_distribution<size_t>
		startDist(0, startParts.size() - 1),
		endDist(0, endParts.size() - 1);

	addPart(startParts.at(startDist(eng_)));

	for (unsigned int n = 0; n < 20; n++)
	{
		std::uniform_int_distribution<size_t> interDist(0, weightedInter.weight() - 1);
		auto* part = weightedInter.getWeighted(interDist(eng_));
		int weight = weightedInter.changeWeight(part, -int(5 * weightedInter.size()));
		weightedInter.addToRandomWeights(abs(weight), eng_);
		addPart(*part);
	}

	addPart(endParts.at(endDist(eng_)));
}

void World::filterFitHelper(const Part& p1, const Part& p2, const Part& test, std::vector<parttuple>& resList)
{
	Part copy(test);
	for (const Connection& prevc : p1.connections)
	{
		unsigned int newcpos = 0;
		for (Connection& newc : copy.connections)
		{
			if (newc.connectstr != prevc.connectstr)
			{
				newcpos++;
				continue;
			}
			movePart(&copy, &newc, &prevc); // TODO -- Only rotate connections to save some time.
			unsigned int cmpnewcpos = 0;
			for (Connection& cmpnewc : copy.connections)
			{
				if (&cmpnewc == &newc)
					continue;
				for (const Connection& cmpprevc : p2.connections)
				{
					if (cmpnewc.connectstr != cmpprevc.connectstr)
						continue;
					else if (Vertex::equals(cmpprevc.origin(), cmpnewc.origin()))
					{
						resList.push_back(std::make_tuple(copy, newcpos, cmpnewcpos, &prevc, &cmpprevc));
						//return; // or maybe not?
					}
				}
				cmpnewcpos++;
			}
			newcpos++;
		}
	}
}

std::vector<parttuple> World::filterFit(const Part& p1, const Part& p2, const std::vector<Part>& cands)
{
	std::vector<parttuple> ret;

	for (const Part& part : cands)
	{
		if (part.connections.size() <= 1)
			continue;
		filterFitHelper(p1, p2, part, ret);
	}
	return ret;
}

std::vector<Part> World::filter(const Connection* connection, const std::vector<Part>& parts)
{
	std::vector<Part> ret;
	for (const Part& part : parts)
	{
		Part copy(part);
		for (Connection& con : copy.connections)
		{
			movePart(&copy, &con, connection);
			if (!testCollisions(&copy))
			{
				ret.emplace_back(copy);
			}
		}
	}
	return ret;
}

World::World()
{
	auto t = std::chrono::system_clock::now();
	eng_.seed(t.time_since_epoch().count());
}

World::~World()
{
}
