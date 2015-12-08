#include "stdafx.h"
#include <sstream>
#include <limits>
#include "BoundingBox.h"
#include "Vector.h"

bool BoundingBox::testCollision(const BoundingBox& lhs, const BoundingBox& rhs)
{
	return (lhs.max.x() > rhs.min.x()
		&& lhs.min.x() < rhs.max.x()
		&& lhs.max.y() > rhs.min.y()
		&& lhs.min.y() < rhs.max.y()
		&& lhs.max.z() > rhs.min.z()
		&& lhs.min.z() < rhs.max.z());
}

std::string BoundingBox::toStr() const
{
	std::ostringstream os;
	os << "(" << min.x() << ", " << min.y() << ", " << min.z() << ")"
		<< " (" << max.x() << ", " << max.y() << ", " << max.z() << ")";
	return os.str();
}

BoundingBox::BoundingBox(void)
{
}

BoundingBox::BoundingBox(const Vector& v) :
	min(Vertex::allmin(v.beg(), v.end())),
	max(Vertex::allmax(v.beg(), v.end()))
{
}

BoundingBox::BoundingBox(const Vertex& mn, const Vertex& mx) :
	min(mn),
	max(mx)
{
}

BoundingBox::~BoundingBox(void)
{
}

