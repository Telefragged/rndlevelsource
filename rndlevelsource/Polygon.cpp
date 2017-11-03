#include "Polygon.h"
#include <algorithm>
Polygon::classification Polygon::classify(const Plane &plane) const
{
	size_t count = points.size();

	size_t front = 0, back = 0, onplane = 0;

	for (const auto& p : points)
	{
		auto test = plane.evaluate(p);

		if (test <= 0) back++;
		if (test >= 0) front++;
		if (test == 0) onplane++;
	}

	if (onplane == count) return Polygon::classification::onPlane;
	if (front == count) return Polygon::classification::front;
	if (back == count) return Polygon::classification::back;
	return Polygon::classification::spanning;
}

Vertex Polygon::origin() const
{
	Vertex orig = { 0, 0, 0 };

	for (const auto &p : points)
		orig += p;

	return orig / points.size();
}

void Polygon::rotate(const Vertex & point, const Matrix3d & rotmat)
{
	for (auto &p : points)
		p.rotateInPlace(point, rotmat);
}

void Polygon::move(const Vertex & v)
{
	for (auto &p : points)
		p = p + v;
}

void Polygon::slice(const Plane &plane)
{
	Polygon front, back;
	slice(plane, back, front);

	points = back.points;
}

bool Polygon::slice(const Plane &plane, Polygon &back, Polygon &front) const
{

	auto classification = classify(plane);

	if (classification != classification::spanning)
	{
		if (classification == classification::back) back = *this;
		else if (classification == classification::front) front = *this;
		return false;
	}

	back.points.clear();
	front.points.clear();

	size_t prev = 0;

	for (size_t i = 0; i <= points.size(); i++)
	{
		size_t index = i % points.size();
		Vertex end = points[index];
		auto c = plane.evaluate(end);

		if (i > 0 && c != 0 && prev != 0 && c != prev)
		{
			Vertex start = points[i - 1];
			Vector line = Vector::diff(start, end);
			double len = line.vec().length();
			Vertex intersect = Plane::intersectPoint(plane, line);
			if (!Vertex::isVertex(intersect))
				throw new std::exception("Expected intersection");
			front.points.push_back(intersect);
			back.points.push_back(intersect);
		}

		if (i < points.size())
		{
			if (c >= 0) front.points.push_back(end);
			if (c <= 0) back.points.push_back(end);
		}

		prev = c;
	}

	return true;
}

Polygon::Polygon()
{
}

Polygon::Polygon(const Plane & p)
{
	auto dir = p.closestAxisToNormal();
	auto tempV = dir == Vertex::unitZ ? -Vertex::unitY : -Vertex::unitZ;

	auto normal = p.normal();

	auto up = tempV.crossProduct(normal).normalize();
	auto right = normal.crossProduct(up).normalize();

	points = {
		p.p1 + right + up,
		p.p1 - right + up,
		p.p1 - right - up,
		p.p1 + right + up
	};

	auto orig = origin();

	std::transform(points.begin(), points.end(), points.begin(), [&orig](Vertex v) {
		return (v - orig) * 10'000'000.0 + orig;
	});

}

Polygon::~Polygon()
{
}
