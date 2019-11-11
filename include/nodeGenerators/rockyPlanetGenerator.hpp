#pragma once
#include "nodeGenerator.hpp"
#include "terrainChunk.hpp"

class rockyPlanetGenerator : public nodeGenerator{
public:
	rockyPlanetGenerator(unsigned int s, unsigned diameter);
	virtual ~rockyPlanetGenerator(){}
	terrainChunk getChunk(const point3Di& p)const override;
private:
	unsigned _diameter;
	
	void fillWater(terrainChunk& c, const point3Di p, int waterLevel) const; // fill chunk c with water on air blocks below waterLevel(nodeHeight), p is the chunk position in chunkSpace
	void populateTrees(terrainChunk& c, const point3Di p, int waterLevel) const; // populate chunk c with trees on the surface, p is the chunk position in chunkSpace
	void placeTree(terrainChunk& c, const point3Di p) const; //place a tree on chunk c on position p (chunkPosition)
};
