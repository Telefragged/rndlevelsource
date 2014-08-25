#include "stdafx.h"
#include <iostream>
#include <cstring>
#include <time.h>
#include "Timer.h"
#include "World.h"
#include "Vector.h"
#include "Angle.h"
#include "SimplexNoise.h"
#include <png.hpp>

#define ARGEQ(A) !strcmp(argv[i], A)

#define M_E 2.71828182846

png::rgb_pixel lerpcolor(png::rgb_pixel a, png::rgb_pixel b, double mod) {
	png::rgb_pixel ret;
	ret.red = a.red + (byte)((b.red - a.red) * mod);
	ret.green = a.green + (byte)((b.green - a.green) * mod);
	ret.blue = a.blue + (byte)((b.blue - a.blue) * mod);

	return ret;
}

int main(int argc, char *argv[]) {
	mictimer::print_on_destruct = false;
	mictimer timer("ms main()", 1000.0);

	std::string outfile = "C:/Users/vetle/Documents/Hammer Mappssz/roomwrite.vmf";
	for (int i = 1; i < argc; i++) {
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

	std::random_device rd;

	std::mt19937 eng(rd());

	SimplexFractal noise(eng, 6, 0.4);

	double maxgrid = 8;

	png::image<png::rgb_pixel> image(2048, 2048);

	mictimer noise_timer;
	noise_timer.start();

	size_t scale = std::max(image.get_width(), image.get_height());

	for (size_t y = 0; y < image.get_height(); y++) {
		for (size_t x = 0; x < image.get_width(); x++) {
			double xscale = static_cast<double>(x) / scale * maxgrid;
			double yscale = static_cast<double>(y) / scale * maxgrid;
			double dot = (noise.getNoise(xscale, yscale) + 1) / 2;

			double scaleddot = dot; // (1 / (1 + std::pow(M_E, -5 * dot)));
			if (scaleddot >= 1.0) scaleddot = 1.0;
			else if (scaleddot < 0.0) scaleddot = 0.0;

			image[y][x] = lerpcolor({ 0, 0, 0 }, { 255, 255, 255 }, scaleddot);
		}

	}

	noise_timer.stop();

	image.write("simplex.png");

	std::cout << (noise_timer.getTime().count() / 1000.0) << "ms\n";

	std::cout << "Per pixel: " << (noise_timer.getTime().count() / 1000.0) / static_cast<double>(image.get_height() * image.get_width()) << "ms\n";
	std::cin.get();

	return 0;
}
