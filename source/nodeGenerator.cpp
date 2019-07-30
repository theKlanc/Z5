#include "nodeGenerator.hpp"
#include "universeNode.hpp"
#include <time.h>

nodeGenerator::nodeGenerator()
{
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetSeed(time(nullptr));
}

nodeGenerator::~nodeGenerator(){}