#include "Polygon.h"
#include <algorithm>
#include <numeric>

Polygon::classification Polygon::classify(const Plane& plane) const
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
	return std::accumulate(points.cbegin(), points.cend(), Vertex{ 0, 0, 0 }) / double(points.size());
}

void Polygon::rotate(const Vertex & point, const Matrix3d & rotmat)
{
	for (auto &p : points)
		p = p.rotate(point, rotmat);
}

void Polygon::move(const Vertex & v)
{
	for (auto &p : points)
		p += v;
}

void Polygon::moveTo(const Vertex & p)
{
	Vertex dist = p - origin();

	for (auto &p : points)
		p += dist;
}

void Polygon::scale(const Vertex &scale)
{
	this->scale(origin(), scale);
}

void Polygon::scale(const Vertex& origin, const Vertex& scale)
{
	for (auto &point : points)
	{
		Vertex vec = Vector::diff(origin, point).vec();
		vec.x(vec.x() * scale.x());
		vec.y(vec.y() * scale.y());
		vec.z(vec.z() * scale.z());

		point = origin + vec;
	}
}

void Polygon::slice(const Plane &plane)
{
	Polygon front, back;
	if (slice(plane, back, front))
		points = back.points;
}

bool Polygon::slice(const Plane& plane, Polygon& back, Polygon& front) const
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

void Polygon::flip()
{
	std::reverse(points.begin(), points.end());
}

void Polygon::roundPoints(size_t precision)
{
	double exp = std::pow(10, precision);
	std::transform(points.begin(), points.end(), points.begin(), [exp](Vertex v) {
		v.x(std::round(v.x() * exp) / exp);
		v.y(std::round(v.y() * exp) / exp);
		v.z(std::round(v.z() * exp) / exp);
		return v;
	});
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
		return (v - orig).normalize() * 10'000'000.0 + orig;
	});

}

Polygon::Polygon(const std::initializer_list<Vertex>& points)
	: points(points)
{
}
