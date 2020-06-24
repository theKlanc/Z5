#pragma once
#include "reactphysics3d/reactphysics3d.h"
#include "entt/entity/registry.hpp"
#include "universeNode.hpp"

enum class physicsType
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
	~physicsEngine() override;
	void updatePhysics(universeNode& universeBase, entt::registry& registry, double dt);
	void onContact(const CollisionCallback::CallbackData& collisionCallbackInfo) override;
	rp3d::PhysicsWorld* getWorld() const;
private:
	static constexpr double _timeStep = 1.0f/config::physicsHz;
	static constexpr double _solverStep = _timeStep / config::physicsSolverIterations;
	double _remainingTime = 0;

	void applyGravity(universeNode& universeBase, entt::registry& registry, double dt);
	void applyThrusters(universeNode& universeBase, double dt);
	void applyBuoyancy(universeNode& universeBase, entt::registry& registry, double dt);
	void applyDrag(universeNode& universeBase, entt::registry& registry, double dt);

	void applyVelocity(universeNode& universeBase, entt::registry& registry, double dt);

	void testCollisionBetweenNodes(universeNode& left, universeNode& right);
	void detectNodeNode(universeNode& universe, double dt);
	void solveNodeNode(universeNode& universe, double dt);

	void detectNodeEntity(universeNode& universeBase, entt::registry& registry, double dt);
	void solveNodeEntity(universeNode& universeBase, entt::registry& registry, double dt);

	void detectEntityEntity(entt::registry& registry, double dt);
	void solveEntityEntity(entt::registry& registry, double dt);
	
	void EntityEntityCallback(const CollisionCallback::CallbackData& collisionCallbackInfo);
	void NodeEntityCallback(const CollisionCallback::CallbackData& collisionCallbackInfo);
	void NodeNodeCallback(const CollisionCallback::CallbackData& collisionCallbackInfo);
	std::shared_ptr<rp3d::PhysicsWorld> _zaWarudo;


	void reparentizeChildren(universeNode& base);
};
