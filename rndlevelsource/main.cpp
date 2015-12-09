#include <random>
#include <iostream>
#include "Part.h"
#include "Matrix.h"
#include "Angle.h"
#include "Vector.h"

int main(int argc, char* argv[])
{
	//std::random_device rd;

	//std::mt19937 eng(rd());

	Part part("test.vmf");

	part.rotate({ 0, 90, 0 });

	std::cout << part;

	return 0;
}
