#include "Axis.h"

#include <boost/spirit/include/qi.hpp>

void Axis::parsestr(std::string_view str)
{
	namespace qi = boost::spirit::qi;

	size_t epos = str.find_first_of(']') + 1;
	if (epos == std::string::npos)
		throw std::exception("Invalid axis string");

	auto fstr = trim(str.substr(0, epos), "[]");
	size_t vpos = fstr.find_last_of(' ');

	auto parstr = fstr.substr(0, vpos);
	v.parsestr(parstr);

	parstr = fstr.substr(vpos + 1);
	qi::parse(parstr.cbegin(), parstr.cend(), qi::double_, trans);
	parstr = str.substr(fstr.length() + 3);
	qi::parse(parstr.cbegin(), parstr.cend(), qi::double_, scale);
}

void Axis::translate(const Vertex& translation)
{
	trans -= (translation.x() * v.x()) / scale;
	trans -= (translation.y() * v.y()) / scale;
	trans -= (translation.z() * v.z()) / scale;
}

std::string Axis::toStr() const
{
	return "[" + v.toStr() + " " + std::to_string(trans) + "] " + std::to_string(scale);
}
