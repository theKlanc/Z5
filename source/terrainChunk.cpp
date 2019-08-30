#include "terrainChunk.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "config.hpp"
#include <cmath>
#include <iostream>
#include "states/state_playing.hpp"

terrainChunk::~terrainChunk() {}

terrainChunk::terrainChunk(const point3Di& p) : _position(p), _loaded(false),
_blocks(config::chunkSize* config::chunkSize* config::chunkSize),
_colliders(config::chunkSize* config::chunkSize* config::chunkSize,
	nullptr)
{
	Services::physicsMutex.lock();
	_collisionBody = (Services::collisionWorld->createCollisionBody(
		rp3d::Transform{
			{(rp3d::decimal)p.x * config::chunkSize, (rp3d::decimal)p.y * config::chunkSize, (rp3d::decimal)p.z * config::chunkSize},
			rp3d::Quaternion::identity()
		}));
	Services::physicsMutex.unlock();
}

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
	Services::physicsMutex.lock();
	if (_colliders[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z] != nullptr)
	{
		_collisionBody->removeCollisionShape(_colliders[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z]);
		_colliders[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z] = nullptr;
	}
	if (b->solid)
	{
		_colliders[(x * config::chunkSize * config::chunkSize) + (y * config::chunkSize) + z] = _collisionBody->addCollisionShape(&_colliderBox, { {(rp3d::decimal)(x + 0.5),(rp3d::decimal)(y + 0.5),(rp3d::decimal)(z + 0.5)},rp3d::Quaternion::identity() });
	}
	Services::physicsMutex.unlock();
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
		updateAllColliders();
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
		//3 5
		//1 3
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

void terrainChunk::updateAllColliders()
{
	Services::physicsMutex.lock();
	int counter = 0;
	if(_collisionBody!=nullptr)
		Services::collisionWorld->destroyCollisionBody(_collisionBody);
	_collisionBody = Services::collisionWorld->createCollisionBody(rp3d::Transform{ {(rp3d::decimal)_position.x * config::chunkSize,(rp3d::decimal)_position.y * config::chunkSize,(rp3d::decimal)_position.z * config::chunkSize},rp3d::Quaternion::identity() });

	_colliders.clear();
	for (int i = 0; i < config::chunkSize; ++i)
	{
		for (int j = 0; j < config::chunkSize; ++j)
		{
			for (int k = 0; k < config::chunkSize; ++k)
			{
				if (_blocks[counter++]->solid)
				{
					_colliders.push_back(_collisionBody->addCollisionShape(&_colliderBox, { {(rp3d::decimal)(i + 0.5),(rp3d::decimal)(j + 0.5),(rp3d::decimal)(k + 0.5)},rp3d::Quaternion::identity() }));
				}
				else
				{
					_colliders.push_back(nullptr);
				}
			}
		}
	}
	Services::physicsMutex.unlock();
}
