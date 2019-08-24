#pragma once
#include "nodeGenerator.hpp"

class spaceshipGenerator : public nodeGenerator{
	public:
	spaceshipGenerator(): nodeGenerator(){}
	spaceshipGenerator(unsigned int s): nodeGenerator(s){}
	virtual ~spaceshipGenerator(){}
	terrainChunk getChunk(const point3Di& p) override;
	unsigned int getHeight(const point2D& p) override;
	private:
};
