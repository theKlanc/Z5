#pragma once
#include "FastNoise/FastNoise.h"
#include "block.hpp"
#include "terrainChunk.hpp"

class nodeGenerator {
public:
	nodeGenerator();
	nodeGenerator(unsigned int s);
	virtual ~nodeGenerator() = 0;
	virtual terrainChunk getChunk(const point3Di& p) = 0;
	virtual baseBlock& getTopBlock(const point2D& p) = 0;
	virtual int getHeight(const point2D& p) = 0;

	friend void to_json(nlohmann::json &j, const nodeGenerator &ng);
protected:
	virtual json getJson() const = 0;
	double getNoise(point2D p) const;
	FastNoise _noiseGenerator;
	unsigned int _seed = 0;
};

void to_json(nlohmann::json &j, const nodeGenerator &ng);