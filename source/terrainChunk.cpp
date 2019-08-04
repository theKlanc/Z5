#include "terrainChunk.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "config.hpp"
#include <cmath>

block &terrainChunk::getBlock(const point3Di &p) {
	return *_blocks[(p.x % config::chunkSize) * config::chunkSize *
						config::chunkSize +
					(p.y % config::chunkSize) * config::chunkSize +
					(p.z % config::chunkSize)];
}

void terrainChunk::setBlock(block *b, const point3Di &p) {
	_blocks[(p.x % config::chunkSize) * config::chunkSize * config::chunkSize +
			(p.y % config::chunkSize) * config::chunkSize +
			(p.z % config::chunkSize)] = b;
}

bool terrainChunk::loaded() const { return _loaded; }

bool terrainChunk::operator==(const terrainChunk &right) const {
	return _position == right._position;
}

bool terrainChunk::operator==(const point3Di &right) const {
	return _position == right;
}

bool terrainChunk::operator!=(const terrainChunk &right) const {
	return !(*this == right);
}

bool terrainChunk::operator!=(const point3Di &right) const {
	return _position != right;
}

const point3Di &terrainChunk::getPosition() { return _position; }

void terrainChunk::load(const std::filesystem::path &file) {
	if (std::filesystem::exists(file)) {
		_loaded = true;
		// LOAD FROM file
	}
}

void terrainChunk::store(const std::filesystem::path &file) {
	_loaded = false;
	if (!std::filesystem::exists(file.parent_path())) {
		std::filesystem::create_directory(file.parent_path());
	}
	// STORE TO file
}
