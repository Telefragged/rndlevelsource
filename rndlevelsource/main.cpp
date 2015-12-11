#include <iostream>
#include "Part.h"
#include "Vector.h"

int main(int argc, char* argv[])
{
	//std::random_device rd;

	//std::mt19937 eng(rd());

	Solid s = Solid::createBox({{64, 64, 64}});

	Solid::slice(s, { "(32, 0, 0) (0, 32, 0) (0, 0, 32)" });

	std::cin.get();

	return 0;
}
