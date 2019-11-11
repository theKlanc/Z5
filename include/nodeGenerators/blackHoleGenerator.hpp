#pragma once
#include "nodeGenerator.hpp"

class blackHoleGenerator : public nodeGenerator{
	public:
	blackHoleGenerator(): nodeGenerator(){}
	virtual ~blackHoleGenerator(){}
	terrainChunk getChunk(const point3Di& p) const;
	private:
};
