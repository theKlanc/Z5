#include "terrainChunk.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "config.hpp"
#include <cmath>

block& terrainChunk::getBlock(const point3Di& p) {
	int x = p.x % config::chunkSize;
	if (x < 0)
		x += config::chunkSize;
	int y = p.y % config::chunkSize;
	if (y < 0)
		y += config::chunkSize;
	int z = p.z % config::chunkSize;
	if (z < 0)
		z += config::chunkSize;
	return *_blocks[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z];
}

void terrainChunk::setBlock(block* b, const point3Di& p) {
	int x = p.x % config::chunkSize;
	if (x < 0)
		x += config::chunkSize;
	int y = p.y % config::chunkSize;
	if (y < 0)
		y += config::chunkSize;
	int z = p.z % config::chunkSize;
	if (z < 0)
		z += config::chunkSize;
	_blocks[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z] = b;
}

bool terrainChunk::loaded() const { return _loaded; }

bool terrainChunk::operator==(const terrainChunk& right) const {
	return _position == right._position;
}

bool terrainChunk::operator==(const point3Di& right) const {
	return _position == right;
}

bool terrainChunk::operator!=(const terrainChunk& right) const {
	return !(*this == right);
}

bool terrainChunk::operator!=(const point3Di& right) const {
	return _position != right;
}

const point3Di& terrainChunk::getPosition() { return _position; }

void terrainChunk::load(const std::filesystem::path& file) {
	if (std::filesystem::exists(file)) {
		_loaded = true;
		// LOAD FROM file
	}
}

void terrainChunk::store(const std::filesystem::path& file) {
	_loaded = false;
	if (!std::filesystem::exists(file.parent_path())) {
		std::filesystem::create_directory(file.parent_path());
	}
	// STORE TO file
}
