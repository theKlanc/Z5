#pragma once
#include "FastNoise/FastNoise.h"
#include <vector>
#include "block.hpp"
#include "terrainChunk.hpp"

class nodeGenerator{
	public:
	nodeGenerator();
	nodeGenerator(int maxHeight);
	terrainChunk getChunk(const point3Di& p);
	private:
	std::vector<block> *_terrainTable;
	FastNoise _noiseGenerator;
	int _maxHeight = 400;
};
