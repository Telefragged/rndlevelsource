#include <iostream>
#include <thread>
#include <tuple>
#include <random>
#include <chrono>
#include <functional>

#include "Part.h"
#include "Angle.h"
#include "Quaternion.h"
#include "World.h"
#include "BoundingBox.h"
#include "Timer.h"

#include "WeightedVector.h"

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/numeric.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

template <class Eng>
Connection *selectRandomConnection(Part& part, Eng &engine)
{
	if (part.connections.reachable() == 0)
		return nullptr;

	std::uniform_int_distribution<ptrdiff_t> dist(0, part.connections.totalWeight() - 1); 

	return &part.connections.getWeighted(dist(engine));
}

void movePart(Part& part, Connection* newc, const Connection* prevc)
{
	Quaternion targetQuat(prevc->angles());

	targetQuat = targetQuat * Quaternion(Angle{ 0, 180, 0 });

	Angle newAngle = newc->angles();

	auto rotmat = newAngle.calcRotation(targetQuat);

	part.rotate(rotmat);
	Vector mov = Vector::diff(newc->origin(), prevc->origin());
	part.move(mov);

	part.connections.setWeight(newc, 0);
}

void scaleToFit(Part& scaleable, Connection* scalec, const Connection* firstc, const Connection* secondc)
{
	double dist = (firstc->originKV() - secondc->originKV()).length();

	scaleable.scaleTo(dist);

	movePart(scaleable, scalec, firstc);
}

int main(int argc, char* argv[])
{
	WeightedVector<Part> vec;

	WeightedVector<Part> scaleables, starts, inters;

	vec.push_back(Part(R"(f:\test\rndmap\room5.vmf)"));
	vec.push_back(Part(R"(f:\test\testmap.vmf)"));
	vec.push_back(Part(R"(f:\test\rndmap\room6.vmf)"));
	vec.push_back(Part(R"(f:\test\rndmap\room1.vmf)"));
	vec.push_back(Part(R"(f:\test\rndmap\room2.vmf)"));

	auto startPred = [](const Part& part)
	{
		return part.connections.size() >= 1
			&& part.countEntities("info_player_start") == 1;
	};

	auto scaleablePred = [](const Part &part)
	{
		bool aligned = part.connections.size() == 2
					   && Vertex::countDifferentAxes(part.connections[0].origin(), part.connections[1].origin()) == 1;

		bool scaleable = part.info ? part.info->scaleable : false;

		return aligned && scaleable;
	};

	auto interPred = [](const Part& part)
	{
		return part.connections.size() >= 2;
	};

	auto startRange = boost::partition<boost::return_found_end>(vec, std::not_fn(startPred));
	boost::copy(startRange, std::back_inserter(starts));
	boost::erase(vec, startRange);

	//auto scaleableRange = boost::partition<boost::return_found_end>(vec, std::not_fn(scaleablePred));
	//boost::copy(scaleableRange, std::back_inserter(scaleables));
	//boost::erase(vec, scaleableRange);

	auto interRange = boost::partition<boost::return_found_end>(vec, std::not_fn(interPred));
	boost::copy(interRange, std::back_inserter(inters));
	boost::erase(vec, interRange);

	std::random_device dev;

	std::mt19937_64 eng(dev());

	std::uniform_real_distribution<> scaleDist(200., 1000.);

	std::vector<Part> world;

	std::uniform_int_distribution<ptrdiff_t> startDist(0, starts.totalWeight() - 1);

	world.emplace_back(starts.getWeighted(startDist(eng)));

	auto &startPart = world.back();

	auto box = startPart.bbox();

	startPart.moveTo((box.max - box.min) / 2);

	for(size_t n = 0; n < 15; n++)
	{
		std::uniform_int_distribution<ptrdiff_t> interDist(0, inters.totalWeight() - 1);

		auto &weightedPart = inters.getWeighted(interDist(eng));

		inters.addToRandomWeights(inters.setWeight(&weightedPart, 0), eng);

		auto &newPart = world.emplace_back(weightedPart);

		if(scaleablePred(newPart) == true)
			newPart.scaleTo(std::round(scaleDist(eng)));

		auto &prevPart = world[world.size() - 2];
		auto prevc = selectRandomConnection(prevPart, eng);
		auto newc = selectRandomConnection(newPart, eng);

		movePart(newPart, newc, prevc);

		prevPart.connections.setWeight(prevc, 0);
	}

	auto part = boost::accumulate(world, Part());

	part.toFile(R"(f:\test\randomworld.vmf)");

	return 0;
}
