#include "stdafx.h"
#include "SimplexNoise.h"
#include <algorithm>

const Vertex SimplexNoise::grad[] =
{
	{ 1, 1, 0 }, { -1, 1, 0 }, { 1, -1, 0 }, { -1, -1, 0 },
	{ 1, 0, 1 }, { -1, 0, 1 }, { 1, 0, -1 }, { -1, 0, -1 },
	{ 0, 1, 1 }, { 0, -1, 1 }, { 0, 1, -1 }, { 0, -1, -1 }
};

SimplexNoise::SimplexNoise(std::mt19937 &eng)
{
	for (int n = 0; n < 256; n++) {
		perm[n] = (char)n;
	}

	std::shuffle(perm.begin(), perm.begin() + 256, eng);

	for (int n = 256; n < 512; n++) {
		perm[n] = perm[n & 255];
	}
}

double SimplexNoise::getNoise(double x, double y)
{
	const double skew2D = 0.5*(std::sqrt(3.0) - 1.0);
	const double unskew2D = (3.0 - std::sqrt(3.0)) / 6.0;

	double s = (x + y) * skew2D;
	int i = static_cast<int>(std::floor(s + x));
	int j = static_cast<int>(std::floor(s + y));
	double t = (i + j) * unskew2D;

	double origx = i - t;
	double origy = j - t;

	Vertex first(x - origx, y - origy, 0);

	int midI = 0, midJ = 0;

	if (first.x() > first.y())
		midI = 1;
	else
		midJ = 1;

	Vertex mid(first.x() - midI + unskew2D, first.y() - midJ + unskew2D, 0);
	Vertex last(first.x() - 1.0 + (2.0 * unskew2D), first.y() - 1.0 + (2.0 * unskew2D), 0);

	int indI = i & 255, indJ = j & 255;

	std::array<int, 3> gradInd;

	gradInd[0] = perm[indI + perm[indJ]] % 12;
	gradInd[1] = perm[indI + midI + perm[indJ + midJ]] % 12;
	gradInd[2] = perm[indI + 1 + perm[indJ + 1]] % 12;

	std::array<double, 3> contribs;

	for (int n = 0; n < 3; n++) {
		Vertex point;
		switch (n) {
		case 0:
			point = first;
			break;
		case 1:
			point = mid;
			break;
		case 2:
			point = last;
			break;
		}
		double temp = 0.5 - (point.x() * point.x()) - (point.y() * point.y());
		if (temp < 0.0)
			contribs[n] = 0;
		else {
			temp *= temp;
			contribs[n] = temp * temp * Vertex::dotProduct2D(grad[gradInd[n]], point);
		}
	}

	return 70.0 * (contribs[0] + contribs[1] + contribs[2]);
}


SimplexNoise::~SimplexNoise(void)
{
}



double SimplexFractal::getNoise(double x, double y)
{
	double result = 0.0;

	for (size_t n = 0; n < octaves_.size(); n++) {
		double freq = std::pow(2, n);
		double amp = std::pow(lacunarity_, n);

		result += octaves_[n].getNoise(x * freq, y * freq) * amp;
	}
	
	return result;
}

SimplexFractal::SimplexFractal(std::mt19937 &eng, size_t octaves, double lacunarity) {
	if (lacunarity < 0 || lacunarity > 1.0) lacunarity = 0.5;
	lacunarity_ = lacunarity;

	octaves_.reserve(octaves);

	for (size_t n = 0; n < octaves; n++) {
		octaves_.emplace_back(eng);
	}
}

SimplexFractal::~SimplexFractal()
{

}