#pragma once
#include <random>
#include <mutex>

#include "entt/entity/registry.hpp"
#include "reactphysics3d/reactphysics3d.h"
#include "graphicsManager.hpp"
#include "fontManager.hpp"
#include "audioManager.hpp"
#include "colliderManager.hpp"

struct Services
{
	static entt::registry* enttRegistry;
	static rp3d::PhysicsCommon physicsCommon;
	static rp3d::PhysicsWorld* physicsWorld;
	static std::mutex physicsMutex;

	static graphicsManager graphics;
	static fontManager fonts;
	static audioManager audio;
	static std::minstd_rand lcg;

	static colliderManager colliders;
};
