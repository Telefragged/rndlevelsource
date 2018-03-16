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
#include <boost/range/algorithm_ext/erase.hpp>

void movePart(Part& part, Connection* newc, const Connection* prevc)
{
	Quaternion targetQuat(prevc->angles());

	targetQuat = targetQuat * Quaternion({ 0, 180, 0 });

	Angle newAngle = newc->angles();

	auto rotmat = newAngle.calcRotation(targetQuat);

	part.rotate(rotmat);
	Vector mov = Vector::diff(newc->origin(), prevc->origin());
	part.move(mov);
}

void scaleToFit(Part& scaleable, Connection* scalec, const Connection* firstc, const Connection* secondc)
{
	double dist = (firstc->originKV() - secondc->originKV()).length();

	scaleable.scaleTo(dist);

	movePart(scaleable, scalec, firstc);
}

int main(int argc, char* argv[])
{
	nanotimer incDiskTimer;

	WeightedVector<Part> vec;

	WeightedVector<Part> scaleables, starts, inters;

	incDiskTimer.start();

	vec.push_back(Part(R"(f:\test\rndmap\room5.vmf)"));
	vec.push_back(Part(R"(f:\test\testmap.vmf)"));
	vec.push_back(Part(R"(f:\test\rndmap\room6.vmf)"));

	nanotimer excDiskTimer;

	excDiskTimer.start();

	auto startPred = [](const Part& part)
	{
		return part.connections.size() >= 1
			&& part.countEntities("info_player_start") == 1;
	};

	auto scaleablePred = [](const Part &part)
	{
		return part.connections.size() == 2
			&& Vertex::countDifferentAxes(part.connections[0].origin(), part.connections[1].origin()) == 1;
	};

	auto interPred = [](const Part& part)
	{
		return part.connections.size() >= 2;
	};

	auto startRange = boost::partition<boost::return_found_end>(vec, std::not_fn(startPred));
	boost::copy(startRange, std::back_inserter(starts));
	boost::erase(vec, startRange);

	auto scaleableRange = boost::partition<boost::return_found_end>(vec, std::not_fn(scaleablePred));
	boost::copy(scaleableRange, std::back_inserter(scaleables));
	boost::erase(vec, scaleableRange);

	auto interRange = boost::partition<boost::return_found_end>(vec, std::not_fn(interPred));
	boost::copy(interRange, std::back_inserter(inters));
	boost::erase(vec, interRange);

	auto start = starts[0];
	auto scaleable = scaleables[0];
	auto end = inters[0];

	auto& startConnection = start.connections.at(0);
	auto& scaleConnection = scaleable.connections.at(0);
	auto& endConnection = end.connections.at(0);

	std::random_device dev;
	std::mt19937 eng(dev());

	std::uniform_real_distribution<> scaleDist(200., 1000.);
	std::uniform_real_distribution<> rotDist(-60., 60.);

	start.rotate(Angle{ 0, rotDist(eng), 0 });
	movePart(end, &endConnection, &startConnection);

	Vertex dir = Vertex::unitX.rotate(startConnection.angles().angleMatrix());

	end.move(dir * std::round(scaleDist(eng)));

	scaleToFit(scaleable, &scaleConnection, &startConnection, &endConnection);
	auto part = (start + scaleable + end);

	excDiskTimer.stop();

	part.toFile(R"(f:\test\ScaleTest.vmf)");

	incDiskTimer.stop();

	std::cout << "   Execution time: " << (double)incDiskTimer.getTime().count() / 1'000'000. << "ms\n";
	std::cout << "Excluding disk IO: " << (double)excDiskTimer.getTime().count() / 1'000'000. << "ms\n";

	return 0;
}
