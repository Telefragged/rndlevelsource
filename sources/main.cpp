#include "stdafx.h"
#include <iostream>
#include <cstring>
#include "Timer.h"
#include "World.h"

#define ARGEQ(A) !strcmp(argv[i], A)

int main(int argc, char *argv[]) {
	mictimer::print_on_destruct = true;
	mictimer timer("ms main()", 1000.0);
	bool compile = false, cleanup = true;
	std::string outfile = "./rndmap/compile/roomwrite.vmf";
	for(int i = 1; i < argc; i++) {
		if(ARGEQ("-compile")) compile = true;
		else if(ARGEQ("-outfile")) outfile = argv[++i];
		else if(ARGEQ("-noclean")) cleanup = false;
	}

	Part p1("./rndmap/room5.vmf");
	Part p2("./rndmap/room6.vmf");
	Part p3("./rndmap/room3.vmf");
	World world;
	world.addPart(p1);
	world.addPart(p2);
	world.addPart(p2);
	world.addPart(p2);
	world.addPart(p3);

	Part p4 = world.collapse();
	p4.toFile(outfile);
	return 0;
}
