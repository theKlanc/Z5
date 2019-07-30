#pragma once
#include "nodeGenerator.hpp"

class spaceshipGenerator : public nodeGenerator{
	public:
	spaceshipGenerator(): nodeGenerator(){}
	virtual ~spaceshipGenerator(){}
	terrainChunk getChunk(const point3Di& p);
	private:
};
