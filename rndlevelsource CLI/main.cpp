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
#include "Vector.h"
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

	targetQuat = targetQuat * Quaternion{ 0, 0, 1, 0 };

	Angle newAngle = newc->angles();

	auto rotmat = newAngle.calcRotation(targetQuat);

	part.rotate(rotmat);
	Vector mov = Vector::diff(newc->origin(), prevc->origin());
	part.move(mov);

	auto iter = boost::find_if(part.connections, [newc](auto &con) {return &con == newc; });

	part.connections.setWeight(iter, 0);
}

void scaleToFit(Part& scaleable, Connection* scalec, const Connection* firstc, const Connection* secondc)
{
	double dist = (firstc->originKV() - secondc->originKV()).length();

	scaleable.scaleTo(dist);

	movePart(scaleable, scalec, firstc);
}

int main(int argc, char* argv[])
{
	//Part p(R"(f:\test\collisions\collision 1.vmf)");

	//std::vector<Solid> lhs, rhs;

	//const auto &worldspawn = *boost::find_if(p.entities, &Entity::entworldcmp);

	//for (auto &solid : worldspawn.solids)
	//{
	//	auto &side = solid.sides[0];

	//	if (side["material"] == "BRICK/BRICKFLOOR001A")
	//		lhs.push_back(solid);
	//	else if (side["material"] == "BRICK/BRICKWALL001A")
	//		rhs.push_back(solid);
	//}

	//std::cout << lhs.size() << ", " << rhs.size() << "\n";

	//for (auto &lhsSolid : lhs)
	//{
	//	for (auto &rhsSolid : rhs)
	//	{
	//		if (Solid::testCollision(lhsSolid, rhsSolid))
	//		{
	//			std::cout << "collision\n";
	//		}
	//	}
	//}

	//std::cin.get();

	//return 0;

	World randomWorld;

	randomWorld.addPart(Part(R"(f:\test\rndmap\room5.vmf)"));
	randomWorld.addPart(Part(R"(f:\test\testmap.vmf)"));
	randomWorld.addPart(Part(R"(f:\test\rndmap\room6.vmf)"));
	randomWorld.addPart(Part(R"(f:\test\rndmap\room1.vmf)"));
	randomWorld.addPart(Part(R"(f:\test\rndmap\room2.vmf)"));
	randomWorld.addPart(Part(R"(f:\test\rndmap\roomT.vmf)"));
	randomWorld.addPart(Part(R"(f:\test\rndmap\roomX.vmf)"));

	randomWorld.buildInitial(30);

	randomWorld.serialize().toFile(R"(f:\test\randomworld.vmf)");

	std::cin.get();

	return 0;
}
