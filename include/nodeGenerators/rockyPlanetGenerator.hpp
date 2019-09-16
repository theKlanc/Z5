#pragma once
#include "nodeGenerator.hpp"
#include "terrainChunk.hpp"

class rockyPlanetGenerator : public nodeGenerator{
public:
	rockyPlanetGenerator(unsigned int s);
	virtual ~rockyPlanetGenerator(){}
	terrainChunk getChunk(const point3Di& p) override;
private:
	void fillWater(terrainChunk& c, const point3Di p, int waterLevel); // fill chunk c with water on air blocks below waterLevel(nodeHeight), p is the chunk position in chunkSpace 
	void populateTrees(terrainChunk& c, const point3Di p); // populate chunk c with trees on the surface, p is the chunk position in chunkSpace
	void placeTree(terrainChunk& c, const point3Di p); //place a tree on chunk c on position p (chunkPosition)
};
