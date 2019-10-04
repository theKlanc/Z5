#pragma once
#include "FastNoise/FastNoise.h"
#include <vector>
#include "block.hpp"
#include "terrainChunk.hpp"

class terrainSection
{
public:
	terrainSection(double noise, int sectionWidth, baseBlock& b, baseBlock* surfaceBlock = nullptr);
	double getNoiseCeiling();
	double getSectionWidth();
	baseBlock& getBlock();
	baseBlock* getSurfaceBlock();

private:
	double _noiseCeiling;
	int _sectionWidth;
	baseBlock& _block;
	baseBlock* _surfaceBlock;

};

class terrainPainter
{
public:
	baseBlock& getBlock(int height, double noise);
	baseBlock& getBlock(int height);
	void addSection(terrainSection s);
	void setEmptyBlock(baseBlock* emptyBlock);
	int getHeight(const double& noise);
private:
	baseBlock* _emptyBlock;
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
