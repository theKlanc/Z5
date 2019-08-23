#pragma once
#include <vector>
#include "block.hpp"
#include "config.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"
#include "services.hpp"

class terrainChunk
{
public:
	terrainChunk(){};
	~terrainChunk();
	terrainChunk(const point3Di& p);
	block& getBlock(const point3Di& p);
	void setBlock(block* b, const point3Di& p);
	void setLoaded();
	bool loaded()const;
	rp3d::CollisionBody* getCollider() const;
	void updateAllColliders();
	
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
	rp3d::CollisionBody* _collisionBody=nullptr;
	std::vector<block*> _blocks;
	std::vector<rp3d::ProxyShape*> _colliders;
	inline static rp3d::BoxShape _colliderBox = rp3d::BoxShape({0.5,0.5,0.5});
};
