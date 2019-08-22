#pragma once
#include "reactPhysics3D/src/reactphysics3d.h"
#include "entt/entity/registry.hpp"

class physicsEngine : public rp3d::CollisionCallback
{
	public:
	physicsEngine();
	~physicsEngine();
	void notifyContact(const CollisionCallbackInfo& collisionCallbackInfo) override;
	rp3d::CollisionWorld* getWorld();
	void setRegistry(entt::registry* reg);
private:
	std::unique_ptr<rp3d::CollisionWorld> _zaWarudo;
	entt::registry* _enttRegistry;
};
