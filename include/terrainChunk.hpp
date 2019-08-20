#pragma once
#include <vector>
#include "block.hpp"
#include "config.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"

class terrainChunk
{
public:
	terrainChunk(){};
	terrainChunk(const point3Di& p) : _position(p), _loaded(false), _blocks(config::chunkSize*config::chunkSize*config::chunkSize){}
	block& getBlock(const point3Di& p);
	void setBlock(block* b, const point3Di& p);
	void setLoaded();
	bool loaded()const;
	rp3d::CollisionBody* getCollider() const;
	
	bool operator== (const terrainChunk& right)const;
	bool operator== (const point3Di& right)const;
	bool operator!= (const terrainChunk& right)const;
	bool operator!= (const point3Di& right)const;
	
	const point3Di& getPosition() const;

	void load(const std::filesystem::path& file,const point3Di& chunkPos);
	void store(std::filesystem::path file);

private:

	void updateColliders();
	point3Di _position;
	bool _loaded = false;
	rp3d::CollisionBody* _collisionBody;
	std::vector<block*> _blocks; //Block refs, we'll use unsigned shorts as IDs from the terrainTable if we need memory  "So big, should be on the heap. So fat, too much for the stack."
};
