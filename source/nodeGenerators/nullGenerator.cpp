#include "nodeGenerators/nullGenerator.hpp"
#include "fdd.hpp"

terrainChunk nullGenerator::getChunk(const point3Di &p)
{
	return terrainChunk();
}

baseBlock &nullGenerator::getTopBlock(const point2D &p)
{
	return baseBlock::terrainTable[0];
}

int nullGenerator::getHeight(const point2D &p)
{
	return 0;
}

nlohmann::json nullGenerator::getJson() const
{
	return json{{"type","null"},{"generator",{ } } };
}

void from_json(const nlohmann::json &j, nullGenerator &ng)
{
	ng = nullGenerator();
}