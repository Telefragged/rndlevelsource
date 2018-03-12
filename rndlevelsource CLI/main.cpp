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

	WeightedVector<const Part> vec;

	vec.push_back(Part(R"(f:\test\rndmap\room5.vmf)"));
	vec.push_back(Part(R"(f:\test\testmap.vmf)"));
	vec.push_back(Part(R"(f:\test\rndmap\room6.vmf)"));

	auto range = vec | boost::adaptors::filtered([](const Part &partPtr) {
		return partPtr.connections.size() == 2
			&& Vertex::countDifferentAxes(partPtr.connections[0].origin(), partPtr.connections[1].origin()) == 1;
	}				 | boost::adaptors::indexed());


	auto &startConnection = start.connections.getIndexed(0);
	auto &endConnection = end.connections.getIndexed(0);
	auto &scaleConnection = scaleable.connections.getIndexed(0);

	movePart(end, &endConnection, &startConnection);

	Vertex dir = Vertex::unitX.rotate(startConnection.angles().angleMatrix());

	end.move(dir * 306.5);

	scaleToFit(scaleable, &scaleConnection, &startConnection, &endConnection);

	auto part = (start + scaleable + end);

	part.moveTo({ 0, 0, 0 });

	part.toFile(R"(f:\test\ScaleTest.vmf)");

	std::cin.get();

	return 0;
}
