#include "nodeGenerators/nullGenerator.hpp"
#include "fdd.hpp"

terrainChunk nullGenerator::getChunk(const point3Di &p) const
{
	return terrainChunk();
}
