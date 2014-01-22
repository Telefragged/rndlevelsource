#include "stdafx.h"
#include <assert.h>
#include "Angle.h"
#include "BoundingBox.h"
#include "Matrix.h"
#include "Vector.h"
#include "World.h"

bool World::testCollisions(Part *partptr) {
	if(partptr == nullptr) return true;
	unsigned int npartn = parts.size(), partn = 0;
	for(const Part &p : parts) {
		if(Part::testCollision(p, *partptr)) {
			printf("Collision room %d and %d\n", partn, npartn);
			printf("[(%s) (%s)] %d collision [(%s) (%s)] %d\n\n",
				p.bbox().min.toStr().c_str(), p.bbox().max.toStr().c_str(), partn,
				partptr->bbox().min.toStr().c_str(), partptr->bbox().max.toStr().c_str(), npartn);
			return true;
		}
		partn++;
	}
	return false;
}

void World::addMaster(const Part &part) {
	master.put(new Part(part));
}

void World::addPart(const Part &part) {
	if(parts.size() == 0) {
		parts.put(new Part(part));
		return;
	}
	Part *prevpart = parts.peek_last();
	Connection *prevc = randConnection(*prevpart);
	if(prevc == nullptr) {
		printf("Failed to add part\n");
		return;
	}
	Part *newpart = new Part(part);
	movePart(newpart, prevc);
	testCollisions(newpart);
	parts.put(newpart);
}

void World::movePart(Part *part, Connection *prevc) {
	Connection *newc = randConnection(*part);
	Angle targetAngle = prevc->angles();
	targetAngle[YAW] += 180.0;
	Angle newAngle = newc->angles();
	Angle rotAngle;
	rotAngle.fromMatrix(newAngle.calcRotation(targetAngle));
	part->rotate(rotAngle);
	Vector mov(newc->origin(), prevc->origin());
	part->move(mov);
}

Part World::collapse() {
	Part ret;
	for(Part &part : parts) {
		ret += part;
	}
	return ret;
}

Connection *World::randConnection(Part &part) {
	Connection *ret = part.connections.getweighted(eng_);
	part.connections.setWeight(ret, 0); //Make sure the connection isn't selected again.
	return ret;
}

World::World(void)
{
	eng_.seed(5);
}

World::~World(void)
{
}
