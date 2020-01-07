#pragma once
#include "nodeGenerator.hpp"

class prefabGenerator : public nodeGenerator{
	public:
	prefabGenerator(): nodeGenerator(){}
	prefabGenerator(unsigned int s): nodeGenerator(s){}
	virtual ~prefabGenerator(){}
	terrainChunk getChunk(const point3Di& p)const override;
};
