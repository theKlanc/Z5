#pragma once
#include "nodeGenerator.hpp"

class nullGenerator : public nodeGenerator{
	public:
	nullGenerator(): nodeGenerator(){}
	virtual ~nullGenerator() override {}
	terrainChunk getChunk(const point3Di& p)const override;
};
