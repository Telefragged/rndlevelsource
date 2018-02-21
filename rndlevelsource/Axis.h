#pragma once
#include "Vertex.h"

class Vertex;

class Axis
{
public:
	Vertex v;
	double trans = 0.0;
	double scale = 0.25;

	void parsestr(const std::string& str);
	void translate(const Vertex& translation);

	std::string toStr() const;

	Axis() = default;
	~Axis() = default;
};

