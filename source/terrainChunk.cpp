#include "terrainChunk.hpp"
#include "config.hpp"
#include <cmath>

block &terrainChunk::getBlock(const point3Di& p)
{
	return *_blocks[
				(p.x%config::chunkSize) * config::chunkSize * config::chunkSize +
				(p.y%config::chunkSize) * config::chunkSize +
				(p.z%config::chunkSize)];
}

void terrainChunk::setBlock(block* b, const point3Di& p)
{
	_blocks[(p.x%config::chunkSize) * config::chunkSize * config::chunkSize +
			(p.y%config::chunkSize) * config::chunkSize +
			(p.z%config::chunkSize)]
			= b;
}

const point3Di &terrainChunk::getPosition()
{
	return _position;
}

void terrainChunk::load(std::ifstream &file)
{

}

void terrainChunk::store(std::ofstream &file)
{

}
