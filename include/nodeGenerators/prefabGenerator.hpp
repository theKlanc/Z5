#pragma once
#include "nodeGenerator.hpp"
#include "prefab.hpp"

class prefabGenerator : public nodeGenerator{
	public:
	prefabGenerator(): nodeGenerator(){}
	prefabGenerator(std::string s);
	~prefabGenerator() override{}
	terrainChunk getChunk(const point3Di& indexedChunkPos) override;
	baseBlock& getTopBlock(const point2D& p) override;
	int getHeight(const point2D& p) override;
	json getJson() const override;

	friend void from_json(const json& j, prefabGenerator &pg);

	prefab _prefab;
};

void from_json(const json& j, prefabGenerator &pg);
