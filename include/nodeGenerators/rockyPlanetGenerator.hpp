#pragma once
#include "nodeGenerator.hpp"

class rockyPlanetGenerator : public nodeGenerator{
	public:
	rockyPlanetGenerator(): nodeGenerator(){}
	rockyPlanetGenerator(unsigned int s): nodeGenerator(s){}
	virtual ~rockyPlanetGenerator(){}
	terrainChunk getChunk(const point3Di& p);
};
