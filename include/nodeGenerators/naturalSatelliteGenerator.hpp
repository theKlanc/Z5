#pragma once
#include "nodeGenerator.hpp"

class naturalSatelliteGenerator : public nodeGenerator{
	public:
	naturalSatelliteGenerator(): nodeGenerator(){}
	virtual ~naturalSatelliteGenerator(){}
	terrainChunk getChunk(const point3Di& p) const;
	private:
};
