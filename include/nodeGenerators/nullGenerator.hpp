#pragma once
#include "nodeGenerator.hpp"

class nullGenerator : public nodeGenerator{
	public:
	nullGenerator(): nodeGenerator(){}
	~nullGenerator() override {}
	terrainChunk getChunk(const point3Di& p)const override;
	baseBlock& getTopBlock(const point2D& p) const override;
	int getHeight(const point2D& p) const override;
};
