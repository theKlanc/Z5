#pragma once
#include "FastNoise/FastNoise.h"
#include <vector>
#include "block.hpp"
#include "terrainChunk.hpp"

class nodeGenerator{
public:
	nodeGenerator();
	virtual ~nodeGenerator() = 0;
	virtual terrainChunk getChunk(const point3Di& p) = 0;
	unsigned int getHeight(const point2D& p);
	protected:
	FastNoise _noiseGenerator;
	unsigned int _maxHeight;
	
};
