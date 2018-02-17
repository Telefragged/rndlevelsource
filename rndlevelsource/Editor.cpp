#include "Editor.h"

#include "utils.h"

unsigned int Editor::parse(std::istream& stream)
{
	unsigned int numparsed = 0;
	std::string curline;
	while (trim(curline) != "{")
	{
		getline(stream, curline);
		numparsed++;
	}
	while (getline(stream, curline))
	{
		numparsed++;
		if (trim(curline) == "}")
		{
			break;
		}
		else
		{
			keyvals.emplace_back(curline);
		}
	}
	return numparsed;
}

Editor::Editor(): depth_(0)
{
}

Editor::~Editor()
{
}

