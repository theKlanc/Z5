#pragma once
#include "nodeGenerator.hpp"

class asteroidGenerator : public nodeGenerator{
	public:
	asteroidGenerator(): nodeGenerator(){}
	virtual ~asteroidGenerator(){}
	terrainChunk getChunk(const point3Di& p, rp3d::RigidBody* rb);
	private:
};
