#pragma once
#include "nodeGenerator.hpp"

class starGenerator : public nodeGenerator{
	public:
	starGenerator(): nodeGenerator(){}
	virtual ~starGenerator(){}
	terrainChunk getChunk(const point3Di& p);
	private:
};
