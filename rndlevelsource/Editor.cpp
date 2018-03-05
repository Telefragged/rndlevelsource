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
			break;

		KeyVal parsed(curline);

		keyvals[parsed.key()] = parsed.val();
	}
	return numparsed;
}

std::string Editor::getName() const
{
	return "editor";
}
