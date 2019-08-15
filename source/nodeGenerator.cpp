#include "nodeGenerator.hpp"
#include "universeNode.hpp"
#include <time.h>

nodeGenerator::nodeGenerator()
{
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetSeed(time(nullptr));
	_maxHeight=400;
}

nodeGenerator::~nodeGenerator(){}

unsigned nodeGenerator::getHeight(const point2D& p)
{
	return ((_noiseGenerator.GetNoise(p.x, p.y ) + 1) / 2)*_maxHeight;
}
