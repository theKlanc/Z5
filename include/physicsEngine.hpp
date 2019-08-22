#pragma once
#include "reactPhysics3D/src/reactphysics3d.h"

class physicsEngine : public rp3d::CollisionCallback
{
	public:
	physicsEngine();
	~physicsEngine();
	void notifyContact(const CollisionCallbackInfo& collisionCallbackInfo) override;
	rp3d::CollisionWorld* getWorld();
private:
	std::unique_ptr<rp3d::CollisionWorld> _zaWarudo;
};