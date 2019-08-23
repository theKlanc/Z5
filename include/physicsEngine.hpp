#pragma once
#include "reactPhysics3D/src/reactphysics3d.h"
#include "entt/entity/registry.hpp"
#include "universeNode.hpp"

enum physicsType
{
	ENTITY,
	NODE,
};

union collidedBody
{
	entt::entity entity;
	universeNode* node;
};

struct collidedResponse
{
	physicsType type;
	collidedBody body;
};

class physicsEngine : public rp3d::CollisionCallback
{
	public:
	physicsEngine();
	~physicsEngine();
	void notifyContact(const CollisionCallbackInfo& collisionCallbackInfo) override;
	rp3d::CollisionWorld* getWorld();
	double dt;
private:
	void solveEntityEntity(const CollisionCallbackInfo& collisionCallbackInfo);
	void solveNodeEntity(const CollisionCallbackInfo& collisionCallbackInfo);
	void solveNodeNode(const CollisionCallbackInfo& collisionCallbackInfo);
	std::unique_ptr<rp3d::CollisionWorld> _zaWarudo;
};
