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
	void processCollisions(universeNode& universeBase, entt::registry& registry, double dt);
	void notifyContact(const CollisionCallbackInfo& collisionCallbackInfo) override;
	rp3d::CollisionWorld* getWorld();
	
private:
	double _dt;
	
	void detectNodeNode(universeNode& universe, double dt);
	void solveNodeNode(universeNode& universe, double dt);

	void detectNodeEntity(universeNode& universeBase, entt::registry& registry, double dt);
	void solveNodeEntity(universeNode& universeBase, entt::registry& registry, double dt);

	void detectEntityEntity(entt::registry& registry, double dt);
	void solveEntityEntity(entt::registry& registry, double dt);
	
	void EntityEntityCallback(const CollisionCallbackInfo& collisionCallbackInfo);
	void NodeEntityCallback(const CollisionCallbackInfo& collisionCallbackInfo);
	void NodeNodeCallback(const CollisionCallbackInfo& collisionCallbackInfo);
	std::unique_ptr<rp3d::CollisionWorld> _zaWarudo;
};
