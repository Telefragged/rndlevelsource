#include "Polygon.h"

#include <algorithm>
#include <numeric>

#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>

#include "Vector.h"
#include "Vertex.h"

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

Plane Polygon::plane() const
{
	if(points.size() < 3)
		return Plane();
	
	return { points[0], points[1], points[2] };
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

void Polygon::sliceThis(const Plane &plane)
{
	auto[back, front] = slice(plane);
	if (!back.points.empty() && !front.points.empty())
		points = back.points;
}

std::pair<Polygon, Polygon> Polygon::slice(const Plane& plane) const
{
	auto classification = classify(plane);

	std::pair<Polygon, Polygon> ret;

	if (classification != classification::spanning)
	{
		if (classification == classification::back) ret.first = *this;
		else if (classification == classification::front) ret.second = *this;
		return ret;
	}

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
			ret.first.points.push_back(intersect);
			ret.second.points.push_back(intersect);
		}

		if (i < points.size())
		{
			if (c <= 0) ret.first.points.push_back(end);
			if (c >= 0) ret.second.points.push_back(end);
		}

		prev = c;
	}

	return ret;
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

Vertex Polygon::intersectPoint(const Vector& line, int flags) const
{
	Plane plane = this->plane();

	auto point = Plane::intersectPoint(plane, line);

	Vertex defaultRet = (flags & lineBoundsFlag::RETURN_END_ON_FAIL) > 0 ? line.end() : Vertex();

	flags = flags & lineBoundsFlag::ALLOW_BOTH;

	if (!Vertex::isVertex(point))
		return defaultRet;

	bool test = testCollision(point);

	if (!test)
		return defaultRet;

	if (flags != lineBoundsFlag::ALLOW_BOTH)
	{
		double position = line.calculatePosition(point);

		if ((lineBoundsFlag::ALLOW_BACK & flags) == 0 && (position < 0 || doubleeq(position, 0)))
			return defaultRet;

		if ((lineBoundsFlag::ALLOW_FRONT & flags) == 0 && (position > 1 || doubleeq(position, 1)))
			return defaultRet;
	}

	return point;
}

bool Polygon::testCollision(const Vertex& point) const
{
	double sum = 0;

	for (size_t n = 0; n < points.size(); n++)
	{
		Vertex p1 = points[n] - point;
		Vertex p2 = points[(n + 1) % points.size()] - point;

		double nom = p1.length() * p2.length();

		if (doubleeq(nom, 0))
			return false;

		sum += acos(p1.dotProduct(p2) / nom);
	}

	return doubleeq(sum, M_PI * 2);
}

bool Polygon::testCollision(const Vector& line, int flags) const
{
	// Ensure method returns NaN vector if it fails so we can test it.
	Vertex intersect = intersectPoint(line, flags);

	if (!Vertex::isVertex(intersect))
		return false;

	return true;
}

bool Polygon::testCollision(const Polygon& polygon) const
{
	if (&polygon == this)
		return true;

	if (points.size() < 3 || polygon.points.size() < 3)
		return false;

	Vertex thisNorm = this->plane().normal();
	Vertex polyNorm = polygon.plane().normal();

	if (thisNorm == polyNorm || thisNorm == -polyNorm)
		return false;

	for (size_t n = 0; n < points.size(); n++)
	{
		Vector line = Vector::diff(points[n], points[(n + 1) % points.size()]);

		if (polygon.testCollision(line, ALLOW_NONE))
			return true;
	}

	for (size_t n = 0; n < polygon.points.size(); n++)
	{
		Vector line = Vector::diff(polygon.points[n], polygon.points[(n + 1) % polygon.points.size()]);

		if (this->testCollision(line, ALLOW_NONE))
			return true;
	}

	return false;
}

Polygon::Polygon(const Plane & p)
{
	auto dir = p.closestAxisToNormal();
	auto tempV = dir == Vertex::unitZ ? -Vertex::unitY : -Vertex::unitZ;

	auto normal = p.normal();

	auto up = tempV.crossProduct(normal).normalize();
	auto right = normal.crossProduct(up).normalize();

	points = {
		p.p1() + right + up,
		p.p1() - right + up,
		p.p1() - right - up,
		p.p1() + right + up
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
