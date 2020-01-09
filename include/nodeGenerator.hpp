#pragma once
#include "FastNoise/FastNoise.h"
#include <vector>
#include "block.hpp"
#include "terrainChunk.hpp"

class nodeGenerator {
public:
	nodeGenerator();
	nodeGenerator(unsigned int s);
	virtual ~nodeGenerator() = 0;
	virtual terrainChunk getChunk(const point3Di& p) const = 0;
	virtual baseBlock& getTopBlock(const point2D& p) const = 0;
	virtual int getHeight(const point2D& p) const = 0;
protected:
	double getNoise(point2D p) const;
	FastNoise _noiseGenerator;
	unsigned int _seed = 0;
};
