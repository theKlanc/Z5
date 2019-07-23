#pragma once
#include <array>
#include "block.hpp"
#include "config.hpp"

class terrainChunk
{
public:
	terrainChunk(const point3Di& p = point3Di()) : _position(p){}
	block& getBlock(const point3Di& p);
	void setBlock(block* b, const point3Di& p);

	const point3Di& getPosition();

	void load(std::ifstream& file);
	void store(std::ofstream& file);
private:

	const point3Di _position;
	bool _loaded = false;
	std::array<block*,(config::chunkSize*config::chunkSize*config::chunkSize)> _blocks; //Block refs, we'll use unsigned shorts as IDs from the terrainTable if we need memory
};
