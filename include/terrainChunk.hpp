#pragma once
#include <vector>
#include "block.hpp"
#include "config.hpp"

class terrainChunk
{
public:
	terrainChunk(const point3Di& p = point3Di()) : _position(p), _blocks(config::chunkSize*config::chunkSize*config::chunkSize){}
	block& getBlock(const point3Di& p);
	void setBlock(block* b, const point3Di& p);

	const point3Di& getPosition();

	void load(std::ifstream& file);
	void store(std::ofstream& file);
private:

	const point3Di _position;
	bool _loaded = false;
	std::vector<block*> _blocks; //Block refs, we'll use unsigned shorts as IDs from the terrainTable if we need memory  "So big, should be on the heap. So fat, too much for the stack."
};
