#include "Axis.h"

void Axis::parsestr(const std::string& str)
{
	size_t epos = str.find_first_of(']') + 1;
	if (epos == std::string::npos)
		throw std::exception("Invalid axis string");

	std::string fstr = trim(str.substr(0, epos), "[]");
	size_t vpos = fstr.find_last_of(' ');

	std::string parstr = fstr.substr(0, vpos);
	v.parsestr(parstr);

	parstr = fstr.substr(vpos + 1);
	trans = atof(parstr.c_str());

	parstr = str.substr(fstr.length() + 3);
	scale = atof(parstr.c_str());
}

void Axis::translate(const Vertex& translation)
{
	trans -= (translation.x() * v.x()) / scale;
	trans -= (translation.y() * v.y()) / scale;
	trans -= (translation.z() * v.z()) / scale;
}

std::string Axis::toStr() const
{
	std::ostringstream os;
	os << "[" << v.toStr() << " " << trans << "] " << scale;
	return os.str();
}

