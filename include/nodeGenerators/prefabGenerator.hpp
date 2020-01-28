#pragma once
#include "nodeGenerator.hpp"

class prefabGenerator : public nodeGenerator{
	public:
	prefabGenerator(): nodeGenerator(){}
	prefabGenerator(unsigned int s): nodeGenerator(s){}
	~prefabGenerator() override{}
	terrainChunk getChunk(const point3Di& p)const override;
	baseBlock& getTopBlock(const point2D& p) const override;
	int getHeight(const point2D& p) const override;
	json getJson() const override;

	friend void from_json(const json& j, prefabGenerator &pg);
};

void from_json(const json& j, prefabGenerator &pg);