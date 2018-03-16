#pragma once
#include "Vertex.h"

class Vertex;

class Axis
{
public:
	Vertex v;
	double trans = 0.0;
	double scale = 0.25;

	void parsestr(std::string_view str);
	void translate(const Vertex& translation);

	std::string toStr() const;

	Axis() = default;
	~Axis() = default;
};

inline std::ostream& operator<<(std::ostream& os, const Axis& a)
{
	os << '[' << a.v << ' ' << a.trans << "] " << a.scale;
	return os;
}
