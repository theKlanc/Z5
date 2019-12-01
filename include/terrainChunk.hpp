#pragma once
#include <vector>
#include "block.hpp"
#include "config.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"
#include "services.hpp"

class terrainChunk
{
public:
	terrainChunk() = default;
	~terrainChunk();
	terrainChunk(const point3Di& p); // THIS TRASH ALLOCATES, avoid destroying without unloading
	metaBlock& getBlock(const point3Di& p);
	void clearDirtyFlag(); // CAREFUL, YOU SHOULD KNOW WHAT YOU'RE DOING
	void setBlock(metaBlock b, const point3Di& p);
	void setLoaded();
	bool loaded()const;
	bool isInside(const point3Dd& point) const;
	bool isValid(const point3Di& nodePos) const;
	rp3d::CollisionBody* getCollider() const;
	void updateAllColliders();

	bool operator== (const terrainChunk& right)const;
	bool operator== (const point3Di& right)const;
	bool operator!= (const terrainChunk& right)const;
	bool operator!= (const point3Di& right)const;

	const point3Di& getIndexedPosition() const;
	point3Di getPosition() const;

	void load(const std::filesystem::path& file, const point3Di& chunkPos);
	void unload(std::filesystem::path file);

private:

	bool _dirty = false;
	point3Di _indexedPosition;
	bool _loaded = false;
	rp3d::CollisionBody* _collisionBody = nullptr;
	std::vector<metaBlock> _blocks;
	std::vector<rp3d::ProxyShape*> _colliders;
	inline static rp3d::BoxShape _colliderBox = rp3d::BoxShape({ 0.5,0.5,0.5 });
};
