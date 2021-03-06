#pragma once
#include "reactPhysics3D/src/reactphysics3d.h"
#include "entt/entity/registry.hpp"
#include "universeNode.hpp"

enum class physicsType
{
	ENTITY,
	PROJECTILE,
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
	void notifyContact(const CollisionCallbackInfo& collisionCallbackInfo) override;
	rp3d::CollisionWorld* getWorld() const;
private:
	static constexpr double _timeStep = 1.0f/config::physicsHz;
	static constexpr double _solverStep = _timeStep / config::physicsSolverIterations;
	double _remainingTime = 0;

	void applyGravity(universeNode& universeBase, entt::registry& registry, double dt);
	void applyThrusters(universeNode& universeBase, double dt);
	void applyBuoyancy(universeNode& universeBase, entt::registry& registry, double dt);
	void applyDrag(universeNode& universeBase, entt::registry& registry, double dt);

	void applyVelocity(universeNode& universeBase, entt::registry& registry, double dt);
	void calculateRPositions(universeNode& universeBase, entt::registry& registry, double dt);

	void testCollisionBetweenNodes(universeNode& left, universeNode& right);
	void detectNodeNode(universeNode& universe, double dt);
	void solveNodeNode(universeNode& universe, double dt);

	void detectNodeEntity(universeNode& universeBase, entt::registry& registry, double dt);
	void solveNodeEntity(universeNode& universeBase, entt::registry& registry, double dt);

	void detectEntityEntity(entt::registry& registry, double dt);
	void solveEntityEntity(entt::registry& registry, double dt);
	
	void EntityEntityCallback(const CollisionCallbackInfo& collisionCallbackInfo);
	void EntityProjectileCallback(const CollisionCallbackInfo& collisionCallbackInfo);
	void EntityNodeCallback(const CollisionCallbackInfo& collisionCallbackInfo);
	void NodeNodeCallback(const CollisionCallbackInfo& collisionCallbackInfo);
	std::unique_ptr<rp3d::CollisionWorld> _zaWarudo;


	void reparentizeNodes(universeNode& base);
	void reparentizeEntities(entt::registry& registry);
};
