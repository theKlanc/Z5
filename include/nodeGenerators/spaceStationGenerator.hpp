#pragma once
#include "nodeGenerator.hpp"

class spaceStationGenerator : public nodeGenerator{
	public:
	spaceStationGenerator(): nodeGenerator(){}
	virtual ~spaceStationGenerator(){}
	terrainChunk getChunk(const point3Di& p);
	private:
};
