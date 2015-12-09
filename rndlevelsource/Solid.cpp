#include "stdafx.h"
#include "BoundingBox.h"
#include "Solid.h"
#include "Vector.h"

bool Solid::testCollision(const Solid& lhs, const Solid& rhs)
{
	if (BoundingBox::testCollision(lhs.bbox(), rhs.bbox()))
	{
		for (const Side& lhss : lhs.sides)
		{
			for (const Side& rhss : rhs.sides)
			{
				if (Side::testCollision(lhss, rhss)) return true;
			}
		}
	}
	return false;
}

Solid Solid::createBox(const Vector& vec, std::string texture, int texturemode, std::string othertexture)
{
	Vertex spos = Vertex::allmin(vec.beg(), vec.end());
	Vertex epos = Vertex::allmax(vec.beg(), vec.end());
	Vertex tpos[4]; //Topmost coordinates. (max z)
	Vertex bpos[4]; //Bottommost coordinates. (min z)
	//clockwise from the top left position on the XY plane.
	// 0 X-------X 1
	//   |       |
	//   |       |
	// 3 X-------X 2
	tpos[0] = Vertex(spos.x(), epos.y(), epos.z()); //top left
	tpos[1] = Vertex(epos.x(), epos.y(), epos.z()); //top right
	tpos[2] = Vertex(epos.x(), spos.y(), epos.z()); //bottom right
	tpos[3] = Vertex(spos.x(), spos.y(), epos.z()); //bottom left
	//Same as over for the bottom coords.
	bpos[0] = Vertex(spos.x(), epos.y(), spos.z());
	bpos[1] = Vertex(epos.x(), epos.y(), spos.z());
	bpos[2] = Vertex(epos.x(), spos.y(), spos.z());
	bpos[3] = Vertex(spos.x(), spos.y(), spos.z());

	Solid box;
	for (unsigned int n = 0; n < 6; n++)
	{
		Side side;
		side["material"] = texture;
		side["rotation"] = "0";
		side["lightmapscale"] = "16";
		side["smoothing_groups"] = "0";
		Plane plane;
		Axis uaxis, vaxis;
		//We fix translation later.
		uaxis.trans = 0.0;
		vaxis.trans = 0.0;

		//scale default is 0.25
		uaxis.scale = 0.25;
		vaxis.scale = 0.25;
		switch (n)
		{
		case 0:
			// top face / UP (XY plane)
			if (texturemode & UP) side["material"] = othertexture;
			plane.p1 = tpos[0];
			plane.p2 = tpos[1];
			plane.p3 = tpos[2];
			uaxis.v = Vertex(1, 0, 0);
			vaxis.v = Vertex(0, -1, 0);
			break;
		case 1:
			// bottom face / DOWN (XY plane)
			if (texturemode & DOWN) side["material"] = othertexture;
			plane.p1 = bpos[3];
			plane.p2 = bpos[2];
			plane.p3 = bpos[1];
			uaxis.v = Vertex(1, 0, 0);
			vaxis.v = Vertex(0, -1, 0);
			break;
		case 2:
			// front face / SOUTH (XZ plane)
			if (texturemode & SOUTH) side["material"] = othertexture;
			plane.p1 = tpos[3];
			plane.p2 = tpos[2];
			plane.p3 = bpos[2];
			uaxis.v = Vertex(1, 0, 0);
			vaxis.v = Vertex(0, 0, -1);
			break;
		case 3:
			// back face / NORTH (XZ plane)
			if (texturemode & NORTH) side["material"] = othertexture;
			plane.p1 = tpos[1];
			plane.p2 = tpos[0];
			plane.p3 = bpos[0];
			uaxis.v = Vertex(1, 0, 0);
			vaxis.v = Vertex(0, 0, -1);
			break;
		case 4:
			// left face / WEST (YZ plane)
			if (texturemode & WEST) side["material"] = othertexture;
			plane.p1 = tpos[0];
			plane.p2 = tpos[3];
			plane.p3 = bpos[3];
			uaxis.v = Vertex(0, 1, 0);
			vaxis.v = Vertex(0, 0, -1);
			break;
		case 5:
			// right face / EAST (YZ plane)
			if (texturemode & EAST) side["material"] = othertexture;
			plane.p1 = tpos[2];
			plane.p2 = tpos[1];
			plane.p3 = bpos[1];
			uaxis.v = Vertex(0, 1, 0);
			vaxis.v = Vertex(0, 0, -1);
			break;
		}
		//translation for pos (0, 0, 0) is 0.0, so we translate it to the start of the vector.
		//uaxis.translate(vec.beg());
		//vaxis.translate(vec.beg());
		side.p = plane;
		side.uaxis = uaxis;
		side.vaxis = vaxis;
		box.sides.push_back(side);
	}
	Editor edt;
	edt.keyvals.put(new KeyVal("color", "192 0 192"));
	edt.keyvals.put(new KeyVal("visgroupshown", "1"));
	edt.keyvals.put(new KeyVal("visgroupautoshown", "1"));
	box.edt = edt;
	return box;
}

std::vector<Solid> Solid::carveBox(const Vector& size, const Solid& initial)
{
	auto sVolume = initial.bbox();
	BoundingBox cVolume(size);

	if (!BoundingBox::testCollision(sVolume, cVolume))
	{
		std::vector<Solid>{initial};
	}

	std::vector<Solid> ret;

	Vertex minBound = sVolume.min, maxBound = sVolume.max;

	if (cVolume.min.z() > sVolume.min.z() && !doubleeq(cVolume.min.z(), sVolume.min.z()))
	{
		Vertex min(sVolume.min);
		Vertex max(sVolume.max.x(), sVolume.max.y(), cVolume.min.z());
		Vector box(min, max);
		minBound.z(max.z());
		ret.push_back(createBox(box));
	}

	if (cVolume.max.z() < sVolume.max.z() && !doubleeq(cVolume.max.z(), sVolume.max.z()))
	{
		Vertex min(sVolume.min.x(), sVolume.min.y(), cVolume.max.z());
		Vertex max(sVolume.max);
		Vector box(min, max);
		maxBound.z(min.z());
		ret.push_back(createBox(box));
	}

	if (cVolume.min.x() > sVolume.min.x() && !doubleeq(cVolume.min.x(), sVolume.min.x()))
	{
		Vertex min = Vertex::allmax(minBound, sVolume.min);
		Vertex max = Vertex::allmin(maxBound, sVolume.max);
		max.x(cVolume.min.x());
		Vector box(min, max);
		minBound.x(max.x());
		ret.push_back(createBox(box));
	}

	if (cVolume.max.x() < sVolume.max.x() && !doubleeq(cVolume.max.x(), sVolume.max.x()))
	{
		Vertex min = Vertex::allmax(minBound, sVolume.min);
		Vertex max = Vertex::allmin(maxBound, sVolume.max);
		min.x(cVolume.max.x());
		Vector box(min, max);
		maxBound.x(min.x());
		ret.push_back(createBox(box));
	}

	if (cVolume.min.y() > sVolume.min.y() && !doubleeq(cVolume.min.y(), sVolume.min.y()))
	{
		Vertex min = Vertex::allmax(minBound, sVolume.min);
		Vertex max = Vertex::allmin(maxBound, sVolume.max);
		max.y(cVolume.min.y());
		Vector box(min, max);
		minBound.y(max.y());
		ret.push_back(createBox(box));
	}

	if (cVolume.max.y() < sVolume.max.y() && !doubleeq(cVolume.max.y(), sVolume.max.y()))
	{
		Vertex min = Vertex::allmax(minBound, sVolume.min);
		Vertex max = Vertex::allmin(maxBound, sVolume.max);
		min.y(cVolume.max.y());
		Vector box(min, max);
		maxBound.y(min.y());
		ret.push_back(createBox(box));
	}

	return ret;
}

unsigned int Solid::parse(std::istream& stream)
{
	unsigned int numparsed = 0;
	std::string curline;
	while (trim(curline) != "{")
	{
		getline(stream, curline);
		numparsed++;
	}
	unsigned int depth = 1;
	while (getline(stream, curline))
	{
		numparsed++;
		if (trim(curline) == "side")
		{
			auto it = sides.emplace(sides.end());
			numparsed += it->parse(stream);
		}
		else if (trim(curline) == "editor")
		{
			numparsed += edt.parse(stream);
		}
		else if (trim(curline) == "}")
		{
			if (--depth == 0) break;
		}
		else
		{
			KeyVal k(curline);
			if (k.key == "id") id_ = k.toInt();
		}
	}
	return numparsed;
}

void Solid::rotate(const Vertex& point, const Matrix& rotmat)
{
	//mictimer rottimer("ms Solid::rotate()", 1000.0);
	for (Side& s : sides)
	{
		s.rotate(point, rotmat);
	}
}

void Solid::move(const Vector& vec)
{
	for (Side& s : sides)
	{
		s.move(vec);
	}
}

void Solid::reID(unsigned int* solidID, unsigned int* sideID)
{
	id_ = ++(*solidID);
	for (Side& side : sides)
	{
		side.reID(sideID);
	}
}

unsigned Solid::depth() const
{
	return depth_;
}

void Solid::depth(unsigned int newDepth) const
{
	depth_ = newDepth;
}

BoundingBox Solid::bbox() const
{
	const double dmax = std::numeric_limits<double>::max();
	const double dmin = -dmax;
	Vertex min(dmax, dmax, dmax);
	Vertex max(dmin, dmin, dmin);
	for (const Side& s : sides)
	{
		BoundingBox sbox = s.bbox();
		min = Vertex::allmin(min, sbox.min);
		max = Vertex::allmax(max, sbox.max);
	}
	return BoundingBox(min, max);
}

Solid::Solid(void) :
	id_(0),
	depth_(0)
{
}

Solid::~Solid(void)
{
}

