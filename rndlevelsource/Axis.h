#pragma once
#include "Vertex.h"

class Vertex;

class Axis
{
public:
	Vertex v;
	double trans;
	double scale;

	void parsestr(const std::string&);
	void translate(const Vertex&);

	std::string toStr() const;

	Axis();
	~Axis();
};

