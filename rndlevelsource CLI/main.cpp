#include <iostream>
#include <thread>
#include <tuple>
#include "Part.h"
#include "Angle.h"
#include "World.h"
#include "BoundingBox.h"

int main(int argc, char* argv[])
{
	//std::random_device rd;

	//std::mt19937 eng(rd());

	//Part part1("f:\\test\\rndmap\\room5.vmf");
	//Part part2("f:\\test\\rndmap\\room6.vmf");

	Part part("f:\\test\\TestMap.vmf");

	auto orig = part.entities[0].origin();

	double targetlen = 1500;

	Vertex diff = Vertex::absolute(part.connections.getIndexed(1)->origin() - part.connections.getIndexed(0)->origin());

	if(!doubleeq(diff.x(), 0))
		diff.x(targetlen / diff.x());
	else
		diff.x(1.0);

	if (!doubleeq(diff.y(), 0))
		diff.y(targetlen / diff.y());
	else
		diff.y(1.0);

	if (!doubleeq(diff.z(), 0))
		diff.z(targetlen / diff.z());
	else
		diff.z(1.0);

	Vertex scale = diff;

	std::cout << diff.toStr() << "\n";

	for(Solid &s : part.entities[0].solids)
		s.scale(scale, orig);

	for (Connection &c : part.connections)
	{
		Vertex vec = Vector::diff(orig, c.origin()).vec();
		vec.x(vec.x() * scale.x());
		vec.y(vec.y() * scale.y());
		vec.z(vec.z() * scale.z());

		c["origin"] = (orig + vec).toStr();

		std::cout << c.origin().toStr() << "\n";
	}

	part.toFile("f:\\test\\ScaledMap.vmf");

	std::cin.get();

	//World testWorld;

	//testWorld.addPart(part1);
	//for(int i = 0; i < 3; i++)
	//	testWorld.addPart(part2);

	//Part part = testWorld.collapse();

	//part.moveTo({ 0, 0, 0 });
	////part.toFile("f:\\test\\test2.vmf");

	//std::cout << part << "\n";

	//auto mat = Matrix3d::rotmatz(90);

	//Vertex vec3{ 1, 0, 0 };

	//std::cout << vec3.toMat() << "\n";

	//std::cout << mat << "\n";

	//for (size_t x = 0; x < mat.x(); x++)
	//{
	//	for (int y = 0; y < mat.y(); y++)
	//	{
	//		printf("%3.2f ", mat[x][y]);
	//	}
	//	printf("\n");
	//}

	//Solid s = part.entities[0].solids[0], front, back;

	//part.moveTo({ 0, 0, 0 });

	//Plane p{ "(0 0 64) (64 0 64) (64 32 32)" };

	//std::vector<Solid> newSolids;

	//auto worldspawn = std::find_if(part.entities.begin(), part.entities.end(), &Entity::entworldcmp);

	//for (auto solid : worldspawn->solids)
	//{
	//	Solid front, back;
	//	solid.slice(p, front, back);
	//	if (front.sides.size() > 0) newSolids.push_back(front);
	//	if (back.sides.size() > 0) newSolids.push_back(back);
	//}

	//worldspawn->solids = newSolids;

	//part.reID();

	//part.toFile("f:\\test\\test2.vmf");

	//s.slice({ "(0 0 32) (64 0 32) (64 32 64)" }, front, back);

	//back.move({ 0, 32, 32 });

	//part.entities[0].solids.clear();
	//part.entities[0].solids.push_back(front);
	//part.entities[0].solids.push_back(back);

	//part.reID();

	//std::cout << part << "\n";

	//Polygon p({ "(0 0 0) (0 64 0) (64 64 0)" });

	//Plane plane{ "(0 0 0) (0 64 0) (64 64 0)" };

	//auto classification = p.classify(plane);

	//std::cout << classification << "\n";

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

	return 0;
}
