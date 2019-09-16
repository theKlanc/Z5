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
	block& getBlock(int height);
	void addSection(terrainSection s);
	void setEmptyBlock(block* emptyBlock);
	int getHeight(const double& noise);
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
	double getNoise(point2D p);
	terrainPainter _terrainPainter;
	FastNoise _noiseGenerator;
	unsigned int _seed = 0;
};
