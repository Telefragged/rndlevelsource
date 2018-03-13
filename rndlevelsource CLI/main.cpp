#include <iostream>
#include <thread>
#include <tuple>
#include <random>
#include <chrono>

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
#include <boost/move/iterator.hpp>

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
	WeightedVector<Part> vec;

	WeightedVector<Part> scaleables, starts, inters;

	vec.push_back(Part(R"(f:\test\rndmap\room5.vmf)"));
	vec.push_back(Part(R"(f:\test\testmap.vmf)"));
	vec.push_back(Part(R"(f:\test\rndmap\room6.vmf)"));

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

	auto startRange = boost::partition<boost::return_begin_found>(vec, startPred);
	boost::copy(startRange, std::back_inserter(starts));
	boost::erase(vec, startRange);

	boost::copy(vec | boost::adaptors::filtered(scaleablePred), std::back_inserter(scaleables));
	boost::erase(vec, boost::remove_if<boost::return_found_end>(vec, scaleablePred));

	boost::copy(vec | boost::adaptors::filtered(interPred), std::back_inserter(inters));
	boost::erase(vec, boost::remove_if<boost::return_found_end>(vec, interPred));

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

	part.toFile(R"(f:\test\ScaleTest.vmf)");

	return 0;
}
