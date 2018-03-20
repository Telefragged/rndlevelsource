#include "World.h"

#include <boost/range/algorithm.hpp>

#include "Angle.h"
#include "Quaternion.h"
#include "Connection.h"
#include "Part.h"

void movePart(Part& part, Connection* newc, const Connection* prevc)
{
	Quaternion targetQuat(prevc->angles());

	targetQuat = targetQuat * Quaternion(Angle{ 0, 180, 0 });

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

World::World()
{
	std::random_device dev;
	eng_.seed(dev());
}

World::~World()
{
}
