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
			keyvals.put(new KeyVal(curline));
		}
	}
	return numparsed;
}

Editor::Editor(void): depth_(0)
{
}

Editor::~Editor(void)
{
}

