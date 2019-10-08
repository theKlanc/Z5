#include "terrainChunk.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "config.hpp"
#include <cmath>
#include <iostream>
#include "states/state_playing.hpp"

terrainChunk::~terrainChunk()
{
	//if (_rigidBody != nullptr) 
	//{
	//	Services::physicsMutex.lock();
	//	Services::collisionWorld->destroyCollisionBody(_rigidBody);
	//	_rigidBody = nullptr;
	//	Services::physicsMutex.unlock();
	//}
}

terrainChunk::terrainChunk(const point3Di& p, rp3d::RigidBody* rb) : _position(p), _loaded(false), _blocks(config::chunkSize* config::chunkSize* config::chunkSize, { &baseBlock::terrainTable[0],UP }), _colliders(config::chunkSize* config::chunkSize* config::chunkSize, nullptr), _parentRigidBody(rb) {}

metaBlock& terrainChunk::getBlock(const point3Di& p) {
	int x = p.x % config::chunkSize;
	if (x < 0)
		x += config::chunkSize;
	int y = p.y % config::chunkSize;
	if (y < 0)
		y += config::chunkSize;
	int z = p.z % config::chunkSize;
	if (z < 0)
		z += config::chunkSize;
	return _blocks[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z];
}

void terrainChunk::setBlock(metaBlock b, const point3Di& p) {
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
		_parentRigidBody->removeCollisionShape(_colliders[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z]);
		_colliders[x * config::chunkSize * config::chunkSize + y * config::chunkSize + z] = nullptr;
	}
	if (b.base->solid)
	{
		_colliders[(x * config::chunkSize * config::chunkSize) + (y * config::chunkSize) + z] = _parentRigidBody->addCollisionShape(&_colliderBox, { {(rp3d::decimal)(x + 0.5) + _position.x * config::chunkSize,(rp3d::decimal)(y + 0.5) + _position.y * config::chunkSize,(rp3d::decimal)(z + 0.5) + _position.z * config::chunkSize},rp3d::Quaternion::identity() }, b.base->mass);
	}
	Services::physicsMutex.unlock();
}

void terrainChunk::setLoaded() {
	_loaded = true;
}

bool terrainChunk::loaded() const {
	return _loaded;
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

void terrainChunk::load(const std::string& fileName, const point3Di& chunkPos) {
	if (std::filesystem::exists(fileName)) {
		_position = chunkPos;
		if (_loaded)
		{
			std::cout << "Loading a new chunk on top of another already loaded one";
		}

		_blocks.clear();
		_blocks.resize(0);

		unsigned blockID;
		blockRotation rotation;
		bool savedMeta;
		unsigned length;

		std::ifstream file(fileName);
		std::string input;
		while (file >> input)
		{
			blockID = std::stoi(input);
			file >> input;
			savedMeta = std::stoi(input);
			if (savedMeta) {
				file >> input;
				rotation = (blockRotation)std::stoi(input);
			}
			else {
				rotation = (blockRotation)(rand() % 4);
			}
			file >> input;
			length = std::stoi(input);
			_blocks.insert(_blocks.end(), length, { &baseBlock::terrainTable[blockID],rotation, savedMeta });
		}
		updateAllColliders();
		setLoaded();
	}
}

void terrainChunk::unload(std::string file) {
	if (_loaded) {
		_loaded = false;
		file = file + "/" + std::to_string(_position.x) + "/" + std::to_string(_position.y) + "/" + std::to_string(_position.z) + ".z5c";
		std::filesystem::create_directories(std::filesystem::path(file).parent_path());
		std::ofstream outputFile(file);
		//.z5c file format:
		//RLE file which contains the IDs of the blocks on the chunk
		//no header
		//5 continuous blocks of dirt(3) followed by 3 blocks of air(1) would be:
		//3 5
		//1 3
		// STORE TO file
		metaBlock lastBlock = _blocks[0];
		unsigned accumulatedLength = 0;
		for (metaBlock& b : _blocks)
		{
			if (b != lastBlock)
			{
				if (lastBlock.saveMeta) {
					outputFile << lastBlock.base->ID << ' ' << lastBlock.saveMeta << ' ' << lastBlock.rotation << ' ' << accumulatedLength << std::endl;
				}
				else
				{
					outputFile << lastBlock.base->ID << ' ' << lastBlock.saveMeta << ' ' << accumulatedLength << std::endl;
				}
				lastBlock = b;
				accumulatedLength = 1;
			}
			else
			{
				accumulatedLength++;
			}
		}
		if (lastBlock.saveMeta) {
			outputFile << lastBlock.base->ID << ' ' << lastBlock.saveMeta << ' ' << lastBlock.rotation << ' ' << accumulatedLength << std::endl;
		}
		else
		{
			outputFile << lastBlock.base->ID << ' ' << lastBlock.saveMeta << ' ' << accumulatedLength << std::endl;
		}
		Services::physicsMutex.lock();
		{
			for (rp3d::ProxyShape* collider : _colliders)
			{
				if (collider != nullptr) {
					_parentRigidBody->removeCollisionShape(collider);
					collider = nullptr;
				}
			}
		}
		Services::physicsMutex.unlock();
	}
}

void terrainChunk::updateAllColliders()
{
	Services::physicsMutex.lock();
	{
		int counter = 0;

		_colliders.clear();
		for (int i = 0; i < config::chunkSize; ++i)
		{
			for (int j = 0; j < config::chunkSize; ++j)
			{
				for (int k = 0; k < config::chunkSize; ++k)
				{
					if (_blocks[counter++].base->solid)
					{
						auto v = _parentRigidBody->addCollisionShape(&_colliderBox, { {(rp3d::decimal)(i + 0.5) + _position.x * config::chunkSize,(rp3d::decimal)(j + 0.5) + _position.y * config::chunkSize,(rp3d::decimal)(k + 0.5) + _position.z * config::chunkSize},rp3d::Quaternion::identity() }, _blocks[counter - 1].base->mass);
						_colliders.push_back(v);
					}
					else
					{
						_colliders.push_back(nullptr);
					}
				}
			}
		}
	}
	Services::physicsMutex.unlock();
}
