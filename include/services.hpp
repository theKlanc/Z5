#pragma once
#include "entt/entity/registry.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"


struct Services
{
	static entt::registry* enttRegistry;
	static rp3d::CollisionWorld* collisionWorld;
};