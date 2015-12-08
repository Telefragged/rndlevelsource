#include "stdafx.h"
#include "Axis.h"

void Axis::parsestr(std::string &pstr) {
	size_t epos = pstr.find_first_of(']')+1;
	if(epos == std::string::npos) return;
	std::string fstr = trim(pstr.substr(0, epos), "[]");
	size_t vpos =  fstr.find_last_of(' ');
	std::string parstr = fstr.substr(0, vpos);
	v.parsestr(parstr);
	parstr = fstr.substr(vpos+1);
	trans = atof(parstr.c_str());
	parstr = pstr.substr(fstr.length() + 3);
	scale = atof(parstr.c_str());
}

void Axis::translate(const Vertex &mov) {
	trans -= (mov.x()*v.x()) / scale;
	trans -= (mov.y()*v.y()) / scale;
	trans -= (mov.z()*v.z()) / scale;
}

std::string Axis::toStr() const {
	std::ostringstream os;
	os<<"["<<v.toStr()<<" "<<trans<<"] "<<scale;
	return os.str();
}

Axis::Axis(void)
{
}

Axis::~Axis(void)
{
}
