#include <iostream>
#include <thread>
#include <boost/multiprecision/mpfr.hpp>
#include "Part.h"
#include "Angle.h"

int main(int argc, char* argv[])
{
	//std::random_device rd;

	//std::mt19937 eng(rd());

	//Part part("test.vmf");
	//part.rotate({ 0, 45, 0 });

	//std::cout << part << "\n";

	Matrix <double, 3, 3> mat;

	mat = Matrix3d::rotmatz(90);

	Vertex vec3{ 1, 0, 0 };

	vec3 = vec3.rotate(mat);

	std::cout << vec3.toStr() << "\n";

	for (size_t x = 0; x < mat.x(); x++)
	{
		for (int y = 0; y < mat.y(); y++)
		{
			printf("%3.2f ", mat[x][y]);
		}
		printf("\n");
	}

	//Solid s = part.entities[0].solids[0], front, back;

	//s.slice({ "(0 0 32) (64 0 32) (64 32 64)" }, front, back);

	//back.move({ 0, 32, 32 });

	//part.entities[0].solids.clear();
	//part.entities[0].solids.push_back(front);
	//part.entities[0].solids.push_back(back);

	//part.reID();

	//std::cout << part << "\n";

	//Polygon p({ "(0 0 64) (0 64 64) (64 64 64)" });

	//Plane plane(p.points[0], p.points[1], p.points[2]);

	//std::cout << plane.toStr() << "\n";

	//Plane p("(0, 0, 64) (0, 64, 64) (64, 64, 64)");

	//Polygon polygon({"(0, 0, 0) (0, 0, 1) (1, 0, 1)"}), front, back;

	/*bool res = polygon.slice(p, front, back);*/

	//for (auto &point : polygon.points)
	//	std::cout << point.toStr() << "\n";

	//std::cout << "\n";

	//for (auto &point : front.points)
	//	std::cout << point.toStr() << "\n";

	//std::cout << "\n";

	//for (auto &point : back.points)
	//	std::cout << point.toStr() << "\n";

	std::cin.get();

	return 0;
}
