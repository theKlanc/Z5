#pragma once
#include "nodeGenerator.hpp"
#include "terrainChunk.hpp"

class rockyPlanetGenerator : public nodeGenerator{
public:
	rockyPlanetGenerator(unsigned int s);
	virtual ~rockyPlanetGenerator(){}
	terrainChunk getChunk(const point3Di& p) override;
private:
	void fillWater(terrainChunk& c, const point3Di p);
};
