#include "DispInfo.h"

#include <cmath>
#include <map>
#include <string>
#include <iostream>

#include "KeyVal.h"
#include "utils.h"

SingleDisp::SingleDisp()
	: normal(0, 0, 1)
	  , offset(0, 0, 0)
	  , offset_normal(0, 0, 1)
{
}

unsigned int DispInfo::parse(std::istream& stream)
{
	unsigned int numparsed = 0;
	std::string curline;
	while (trim(curline) != "{")
	{
		getline(stream, curline);
		numparsed++;
	}
	unsigned int depth = 1;
	std::map<std::string, std::map<std::string, std::string>> tempKeys;
	while (getline(stream, curline))
	{
		numparsed++;
		if (trim(curline) == "}")
		{
			std::cout << "IS THIS REAL LIFE??\n";
			if (--depth == 0)
				break;
		}
		else if (!trim(curline).empty() && trim(curline)[0] != '\"')
		{
			auto cpos = stream.tellg();
			std::string peekline;
			getline(stream, peekline);
			stream.seekg(cpos);
			if (trim(peekline) == "{")
			{
				std::string groupName = trim(curline);
				std::map<std::string, std::string> group;
				while (getline(stream, curline))
				{
					numparsed++;
					if (trim(curline) == "}")
						break;

					KeyVal k(curline);
					group[k.key] = k.val;
				}
				tempKeys[groupName] = move(group);
			}
		}
		else if (trim(curline) == "{")
		{
			++depth;
		}
		else
		{
			KeyVal parsed(curline);
			keyvals[parsed.key] = parsed.val;
		}
	}

	int power = atoi(keyvals["power"].c_str());
	int size = (int)pow(2, power) + 1;
	info = std::vector<std::vector<SingleDisp>>(size, std::vector<SingleDisp>(size, SingleDisp()));

	for (int row = 0; row < size; row++)
	{
		std::string rowID = "row";
		rowID += std::to_string(row);
		auto norm = splitstr(tempKeys["normals"][rowID]);
		auto dist = splitstr(tempKeys["distances"][rowID]);
		auto off = splitstr(tempKeys["offsets"][rowID]);
		auto off_norm = splitstr(tempKeys["offset_normals"][rowID]);
		auto alpha = splitstr(tempKeys["alphas"][rowID]);

		for (int col = 0; col < size; col++)
		{
			auto& curInfo = info[row][col];
			size_t vertInd = 3 * col;
			curInfo.normal = Vertex(norm[vertInd] + ' ' + norm[vertInd + 1] + ' ' + norm[vertInd + 2]);
			curInfo.distance = atof(dist[col].c_str());
			curInfo.offset = Vertex(off[vertInd] + ' ' + off[vertInd + 1] + ' ' + off[vertInd + 2]);
			curInfo.offset_normal = Vertex(off_norm[vertInd] + ' ' + off_norm[vertInd + 1] + ' ' + off_norm[vertInd + 2]);
			curInfo.alpha = (char)atoi(alpha[col].c_str());
		}
	}

	return numparsed;
}

void DispInfo::setHeight(int x, int y, double height)
{
	auto& disp = info[y][x];
	disp.distance = abs(height);
	if (height < 0.0)
		disp.normal = Vertex(0, 0, -1);
	else
		disp.normal = Vertex(0, 0, 1);
}

void DispInfo::rotate(const Vertex& point, const Matrix3d& rotmat)
{
	//mictimer rotatetimer("ms Side::rotate()", 1000.0);

	if (keyvals.empty()) return;

	Vertex startpos((*this)["startposition"]);
	startpos = startpos.rotate(point, rotmat);
	(*this)["startposition"] = std::string("[") + startpos.toStr() + "]";

	for (auto& vec : info)
		for (auto& disp : vec)
			disp.offset_normal = disp.offset_normal.rotate(rotmat);
}


