#include "nodeGenerator.hpp"
#include "universeNode.hpp"
#include <time.h>

nodeGenerator::nodeGenerator()
{
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetSeed(_seed);
	_maxHeight=400;
}

nodeGenerator::nodeGenerator(unsigned int s)
{
	_seed = s;
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetFractalGain(0.4);
	_noiseGenerator.SetFrequency(0.001f);
	_noiseGenerator.SetFractalLacunarity(3);
	_noiseGenerator.SetFractalOctaves(5);
	_noiseGenerator.SetSeed(_seed);
	_maxHeight=400;
}

nodeGenerator::~nodeGenerator(){}

unsigned nodeGenerator::getHeight(const point2D& p)
{
	return ((_noiseGenerator.GetNoise(p.x, p.y ) + 1) / 2)*_maxHeight;
}
