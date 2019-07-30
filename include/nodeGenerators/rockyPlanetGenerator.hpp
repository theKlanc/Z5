#pragma once
#include "nodeGenerator.hpp"

class rockyPlanetGenerator : public nodeGenerator{
	public:
	rockyPlanetGenerator(): nodeGenerator(){}
	virtual ~rockyPlanetGenerator(){}
	terrainChunk getChunk(const point3Di& p);
	private:
};
