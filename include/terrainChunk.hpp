#pragma once
#include <vector>
#include "block.hpp"
#include "config.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"
#include "states/state_playing.hpp"

class terrainChunk
{
public:
	terrainChunk(){};
	terrainChunk(const point3Di& p) : _position(p), _loaded(false), _blocks(config::chunkSize* config::chunkSize* config::chunkSize), _collisionBody(rp3d::CollisionBody(rp3d::Transform{{(rp3d::decimal)p.x,(rp3d::decimal)p.y,(rp3d::decimal)p.z},rp3d::Quaternion::identity()})) {}
	block& getBlock(const point3Di& p);
	void setBlock(block* b, const point3Di& p);
	void setLoaded();
	bool loaded()const;
	rp3d::CollisionBody* getCollider() const;
	void updateColliders();
	
	bool operator== (const terrainChunk& right)const;
	bool operator== (const point3Di& right)const;
	bool operator!= (const terrainChunk& right)const;
	bool operator!= (const point3Di& right)const;
	
	const point3Di& getPosition() const;

	void load(const std::filesystem::path& file,const point3Di& chunkPos);
	void store(std::filesystem::path file);

private:

	point3Di _position;
	bool _loaded = false;
	rp3d::CollisionBody _collisionBody;
	std::vector<block*> _blocks; //Block refs, we'll use unsigned shorts as IDs from the terrainTable if we need memory  "So big, should be on the heap. So fat, too much for the stack."
	std::vector<bool> _colliders;
	inline static rp3d::BoxShape _colliderBox = rp3d::BoxShape({0.5,0.5,0.5});
};
