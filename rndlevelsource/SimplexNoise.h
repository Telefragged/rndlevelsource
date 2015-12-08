#pragma once

#include "Vertex.h"
#include <array>
#include <random>
#include <vector>

class SimplexNoise
{
private:
	static const Vertex grad[];
	std::array<unsigned char, 512> perm;

public:

	double getNoise(double x, double y);

	SimplexNoise(std::mt19937&);
	~SimplexNoise();
};

class SimplexFractal
{
private:
	std::vector<SimplexNoise> octaves_;
	double lacunarity_;

public:

	double getNoise(double x, double y);

	SimplexFractal(std::mt19937&, size_t octaves, double lacunarity);
	~SimplexFractal();
};

