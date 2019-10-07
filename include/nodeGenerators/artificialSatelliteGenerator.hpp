#pragma once
#include "nodeGenerator.hpp"

class artificialSatelliteGenerator : public nodeGenerator{
	public:
	artificialSatelliteGenerator(): nodeGenerator(){}
	virtual ~artificialSatelliteGenerator(){}
	terrainChunk getChunk(const point3Di& p, rp3d::RigidBody* rb);
	private:
};
