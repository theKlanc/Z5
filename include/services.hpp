#pragma once
#include "entt/entity/registry.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"
#include <mutex>


struct Services
{
	static entt::registry* enttRegistry;
	static rp3d::CollisionWorld* collisionWorld;
	static std::mutex physicsMutex;
};
