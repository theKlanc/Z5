#pragma once
#include "nodeGenerator.hpp"

class spaceStationGenerator : public nodeGenerator{
	public:
	spaceStationGenerator(){}
	spaceStationGenerator(unsigned int seed);
	virtual ~spaceStationGenerator(){}
	terrainChunk getChunk(const point3Di& p)const;
	private:
};
