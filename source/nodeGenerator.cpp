#include "nodeGenerator.hpp"
#include "universeNode.hpp"
#include <time.h>

nodeGenerator::nodeGenerator()
{
	_noiseGenerator.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
	_noiseGenerator.SetSeed(_seed);
}

nodeGenerator::nodeGenerator(unsigned int s)
{
	_seed = s;
	_noiseGenerator.SetSeed(_seed);
}

nodeGenerator::~nodeGenerator() {}

double nodeGenerator::getNoise(point2D p)
{
	return (_noiseGenerator.GetNoise(p.x, p.y) + 1) / 2;
}


terrainSection::terrainSection(double noise, int sectionWidth, baseBlock& b, baseBlock* surfaceBlock) : _block(b)
{
	_noiseCeiling = noise;
	_sectionWidth = sectionWidth;
	_surfaceBlock = surfaceBlock;
}

double terrainSection::getNoiseCeiling()
{
	return _noiseCeiling;
}

double terrainSection::getSectionWidth()
{
	return _sectionWidth;
}

baseBlock& terrainSection::getBlock()
{
	return _block;
}

baseBlock* terrainSection::getSurfaceBlock()
{
	return _surfaceBlock;
}

baseBlock& terrainPainter::getBlock(int height, double noise)
{
	auto it = _terrainList.begin();
	int accumulatedHeight = 0;
	double lastNoise = 0;
	while (true) // while we haven't found the baseBlock
	{
		if (accumulatedHeight + it->getSectionWidth() < height) // if this section is too low
		{
			accumulatedHeight += it->getSectionWidth();
			lastNoise = it->getNoiseCeiling();
			if (++it == _terrainList.end())//advance to next section, if end then exit
			{
				break;
			}
		}
		else
		{// we're in this section, calculate if we're below noise
			int floorHeight = (noise - lastNoise) / (it->getNoiseCeiling() - lastNoise) * it->getSectionWidth() + accumulatedHeight;
			if (floorHeight > height)
			{
				return it->getBlock();
			}
			if (floorHeight == height)
			{
				if (it->getSurfaceBlock() != nullptr)
				{
					return *it->getSurfaceBlock();
				}
				else
					return it->getBlock();
			}
			else
			{
				return *_emptyBlock;
			}
		}
	}
	return *_emptyBlock;
}

baseBlock& terrainPainter::getBlock(int height)
{
	auto it = _terrainList.begin();
	int accumulatedHeight = 0;
	while (true) // while we haven't found the height
	{
		if (height <= accumulatedHeight + it->getSectionWidth()) // if we belong to this section
		{
			if (height == accumulatedHeight + it->getSectionWidth())
			{
				if (it->getSurfaceBlock() != nullptr)
				{
					return *it->getSurfaceBlock();
				}
			}
			return it->getBlock();
		}
		else //advance
		{
			accumulatedHeight += it->getSectionWidth();
			it++;
		}
	}
}

int terrainPainter::getHeight(const double& noise)
{
	auto it = _terrainList.begin();
	auto oldSection = *it;
	int accumulatedHeight = 0;
	double lastNoise = 0;
	assert(noise <= 1);
	while (true) // while we haven't found the height
	{
		if (noise <= it->getNoiseCeiling()) // if we belong to this section
		{
			return (noise - lastNoise) / (it->getNoiseCeiling() - lastNoise) * it->getSectionWidth() + accumulatedHeight;
		}
		else //advance
		{
			accumulatedHeight += it->getSectionWidth();
			lastNoise = it->getNoiseCeiling();
			it++;
		}
	}
}

void terrainPainter::addSection(terrainSection s)
{
	_terrainList.push_back(s);
}

void terrainPainter::setEmptyBlock(baseBlock* emptyBlock)
{
	_emptyBlock = emptyBlock;
}

baseBlock& nodeGenerator::getTopBlock(const point2D& p)
{
	return _terrainPainter.getBlock(_terrainPainter.getHeight(getNoise(p)));
}
