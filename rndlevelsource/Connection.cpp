#include "Connection.h"
#include <boost/range/algorithm.hpp>

#include "Angle.h"
#include "Quaternion.h"
#include "Part.h"
#include "Vector.h"

void Connection::connectTo(Connection *targetCon)
{
	Quaternion targetQuat(targetCon->angles());

	targetQuat = targetQuat * Quaternion{ 0, 0, 1, 0 };

	Angle newAngle = angles();

	auto rotmat = newAngle.calcRotation(targetQuat);

	parent->rotate(rotmat);
	Vector mov = Vector::diff(origin(), targetCon->origin());
	parent->move(mov);

	neighbour = targetCon;
	targetCon->neighbour = this;
}

bool Connection::isConnected() const
{
	return neighbour != nullptr;
}

Connection::Connection(Entity&& entity)
{
	this->solids = std::move(entity.solids);
	this->keyvals = std::move(entity.keyvals);
	if (keyvals.count("classname") > 0)
	{
		std::string val = keyvals["classname"];

		if (val.length() > 12)
			connectstr = val.substr(12);
	}
}
