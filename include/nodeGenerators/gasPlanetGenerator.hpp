#pragma once
#include "nodeGenerator.hpp"

class gasPlanetGenerator : public nodeGenerator{
	public:
	gasPlanetGenerator(): nodeGenerator(){}
	virtual ~gasPlanetGenerator(){}
	terrainChunk getChunk(const point3Di& p) const;
	private:
};
