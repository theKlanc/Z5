#pragma once
#include "FastNoise/FastNoise.h"
#include <vector>
#include "block.hpp"
#include "terrainChunk.hpp"

class terrainSection
{
public:
	terrainSection(double noise, int sectionWidth, block& b, block* surfaceBlock = nullptr);
	double getNoiseCeiling();
	double getSectionWidth();
	block& getBlock();
	block* getSurfaceBlock();

private:
	double _noiseCeiling;
	int _sectionWidth;
	block& _block;
	block* _surfaceBlock;

};

class terrainPainter
{
public:
	block& getBlock(int height, double noise);
	void addSection(terrainSection s);
	void setEmptyBlock(block* emptyBlock);
private:
	block* _emptyBlock;
	std::vector<terrainSection> _terrainList;
};

class nodeGenerator {
public:
	nodeGenerator();
	nodeGenerator(unsigned int s);
	virtual ~nodeGenerator() = 0;
	virtual terrainChunk getChunk(const point3Di& p) = 0;

protected:
	terrainPainter _terrainPainter;
	FastNoise _noiseGenerator;
	unsigned int _seed = 0;
};
