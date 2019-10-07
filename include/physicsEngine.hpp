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

class physicsEngine
{
public:
	physicsEngine();
	rp3d::DynamicsWorld* getWorld();
	void processCollisions(universeNode& universeBase, entt::registry& registry, double dt);

private:
	double _dt;
	double _remainingTime = 0;
	const double _physicsStep = 1.0f/60.0f;
	std::unique_ptr<rp3d::DynamicsWorld> _zaWarudo;
};
