#include "nodeGenerator.hpp"
#include "universeNode.hpp"
#include <time.h>

nodeGenerator::nodeGenerator()
{
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetSeed(_seed);
}

nodeGenerator::nodeGenerator(unsigned int s)
{
	_seed = s;
	_noiseGenerator.SetSeed(_seed);
}

nodeGenerator::~nodeGenerator() {}

double nodeGenerator::getNoise(point2D p) const
{
	return (_noiseGenerator.GetNoise(p.x, p.y) + 1) / 2;
}