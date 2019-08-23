#pragma once
#include "nodeGenerator.hpp"
#include "terrainChunk.hpp"

class rockyPlanetGenerator : public nodeGenerator{
	public:
	rockyPlanetGenerator(): nodeGenerator(){}
	rockyPlanetGenerator(unsigned int s): nodeGenerator(s){}
	virtual ~rockyPlanetGenerator(){}
	terrainChunk getChunk(const point3Di& p) override;
};
