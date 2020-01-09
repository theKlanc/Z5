#include "nodeGenerators/nullGenerator.hpp"
#include "fdd.hpp"

terrainChunk nullGenerator::getChunk(const point3Di &p) const
{
	return terrainChunk();
}

baseBlock &nullGenerator::getTopBlock(const point2D &p) const
{
	return baseBlock::terrainTable[0];
}

int nullGenerator::getHeight(const point2D &p) const
{
	return 0;
}
