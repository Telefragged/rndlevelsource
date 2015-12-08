#include "stdafx.h"
#include <iostream>
#include <cstring>
#include "Timer.h"
#include "World.h"

#define ARGEQ(A) !strcmp(argv[i], A)

#define M_E 2.71828182846

int main(int argc, char* argv[])
{
	mictimer::print_on_destruct = false;
	mictimer timer("ms main()", 1000.0);

	std::string outfile = "C:/Users/vetle/Documents/Hammer Mappssz/roomwrite.vmf";

	for (int i = 1; i < argc; i++)
	{
		if (ARGEQ("-outfile")) outfile = argv[++i];
	}

	Part p1("./rndmap/room1.vmf");
	Part p2("./rndmap/room2.vmf");
	Part p3("./rndmap/room3.vmf");
	Part p4("./rndmap/corner1.vmf");
	Part p5("./rndmap/room3way.vmf");

	World world;

	world.addMaster(p1, p2, p3, p4, p5);

	//world.buildWorld();

	//Part p6 = world.collapse();

	//p6.toFile(outfile);

	std::cin.get();

	return 0;
}

