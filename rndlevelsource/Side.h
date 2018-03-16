#pragma once

#include <ios>
#include <iomanip>
#include <optional>

#include "Axis.h"
#include "DispInfo.h"
#include "KeyValBase.h"
#include "Plane.h"
#include "Polygon.h"

#ifndef TABDEPTH
#define TABDEPTH 1U
#endif

class BoundingBox;

class Side :
	public KeyValBase
{
private:
public:
	std::optional<DispInfo> disp;
	Axis uaxis, vaxis;
	Polygon polygon;
	//Plane p;

	size_t parseSpecial(std::istream& stream, std::string_view type) override;

	Plane plane() const;

	BoundingBox bbox() const;

	void popuvars();
	void rotate(const Vertex& point, const Matrix3d& rotmat);
	void move(const Vector& vec);
	void reID(unsigned int& sideID);

	static bool testCollision(const Side& lhs, const Side& rhs);

	void extraOutput(std::ostream& os) const override;
	std::string getName() const override;

	Side() = default;
	~Side() override = default;
};
