#include "terrainChunk.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "config.hpp"
#include <cmath>
#include <iostream>
#include "states/state_playing.hpp"

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

void terrainChunk::setLoaded() {
	_loaded = true;
}

bool terrainChunk::loaded() const {
	return _loaded;
}

rp3d::CollisionBody* terrainChunk::getCollider() const {
	return _collisionBody;
}

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

const point3Di& terrainChunk::getPosition() const {
	return _position;
}

void terrainChunk::load(const std::filesystem::path& fileName, const point3Di& chunkPos) {
	if (std::filesystem::exists(fileName)) {
		_position = chunkPos;
		if (_loaded)
		{
			std::cout << "Loading a new chunk on top of another already loaded one";
		}

		_blocks.clear();
		_blocks.resize(0);

		unsigned blockID;
		unsigned length;

		std::ifstream file(fileName);
		std::string input;
		while (file >> input)
		{
			blockID = std::stoi(input);
			file >> input;
			length = std::stoi(input);
			_blocks.insert(_blocks.end(), length, &block::terrainTable[blockID]);
		}

		setLoaded();

	}
}

void terrainChunk::store(std::filesystem::path file) {
	if (_loaded) {
		_loaded = false;
		file.append(std::to_string(_position.x)).append(std::to_string(_position.y)).append(std::to_string(_position.z)).concat(".z5c");
		if (!std::filesystem::exists(file.parent_path())) {
			std::filesystem::create_directories(file.parent_path());
		}
		std::ofstream outputFile(file);
		//.z5c file format:
		//RLE file which contains the IDs of the blocks on the chunk
		//no header
		//5 continuous blocks of dirt(3) followed by 3 blocks of air(1) would be:
		//3:4;1:3;
		// STORE TO file
		unsigned lastBlockID = _blocks[0]->ID;
		unsigned accumulatedLength = 0;
		for (block* b : _blocks)
		{
			if (b->ID != lastBlockID)
			{
				outputFile << lastBlockID << ' ' << accumulatedLength << std::endl;
				lastBlockID = b->ID;
				accumulatedLength = 1;
			}
			else
			{
				accumulatedLength++;
			}
		}
		outputFile << lastBlockID << ' ' << accumulatedLength << std::endl;
	}
}
