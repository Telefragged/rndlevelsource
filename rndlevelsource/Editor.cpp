#include "stdafx.h"
#include "Editor.h"
#include <iostream>

unsigned int Editor::parse(std::istream &stream) {
	unsigned int numparsed = 0;
	std::string curline;
	while(trim(curline) != "{") {
		std::getline(stream, curline);
		numparsed++;
	}
	while (std::getline(stream, curline)) {
		numparsed++;
		if(trim(curline) == "}") {
			break;
		} else {
			keyvals.put(new KeyVal(curline));
		}
	}
	return numparsed;
}

Editor::Editor(void)
{
}

Editor::~Editor(void)
{
}
