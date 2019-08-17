#pragma once
#include "nodeGenerator.hpp"

class spaceshipGenerator : public nodeGenerator{
	public:
	spaceshipGenerator(): nodeGenerator(){}
	spaceshipGenerator(unsigned int s): nodeGenerator(s){}
	virtual ~spaceshipGenerator(){}
	terrainChunk getChunk(const point3Di& p) override;
	private:
};
