#pragma once
#include <random>
#include <mutex>

#include "entt/entity/registry.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"
#include "graphicsManager.hpp"
#include "fontManager.hpp"
#include "audioManager.hpp"

struct Services
{
	static entt::registry* enttRegistry;
	static rp3d::CollisionWorld* collisionWorld;
	static std::mutex physicsMutex;

	static graphicsManager graphics;
	static fontManager fonts;
	static audioManager audio;
	static std::minstd_rand lcg;
};
