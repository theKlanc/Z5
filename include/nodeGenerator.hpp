#pragma once
#include "FastNoise/FastNoise.h"
#include <vector>
#include "block.hpp"
#include "terrainChunk.hpp"

class terrainSection
{
public:
	terrainSection(double noise, int sectionWidth, baseBlock& b, baseBlock* surfaceBlock = nullptr);
	double getNoiseCeiling() const;
	double getSectionWidth() const;
	baseBlock& getBlock() const;
	baseBlock* getSurfaceBlock() const;

private:
	double _noiseCeiling;
	int _sectionWidth;
	baseBlock& _block;
	baseBlock* _surfaceBlock;

};

class terrainPainter
{
public:
	baseBlock& getBlock(int height, double noise) const;
	baseBlock& getBlock(int height) const;
	void addSection(terrainSection s);
	void setEmptyBlock(baseBlock* emptyBlock);
	int getHeight(const double& noise) const;
private:
	baseBlock* _emptyBlock;
	std::vector<terrainSection> _terrainList;
};

class nodeGenerator {
public:
	nodeGenerator();
	nodeGenerator(unsigned int s);
	virtual ~nodeGenerator() = 0;
	virtual terrainChunk getChunk(const point3Di& p) const = 0;
	virtual baseBlock& getTopBlock(const point2D& p) const;
	virtual int getHeight(const point2D& p) const;
protected:
	double getNoise(point2D p) const;
	terrainPainter _terrainPainter;
	FastNoise _noiseGenerator;
	unsigned int _seed = 0;
};
