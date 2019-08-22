#pragma once
#include "physicsEngine.hpp"
#include <iostream>
#include "reactPhysics3D/src/reactphysics3d.h"
#include "reactPhysics3D/src/collision/ContactManifold.h"
#include "reactPhysics3D/src/constraint/ContactPoint.h"
#include "entt/entity/fwd.hpp"
#include "states/state_playing.hpp"
#include "components/velocity.hpp"
#include "components/body.hpp"


physicsEngine::physicsEngine()
{
	//Create collision world
	rp3d::WorldSettings collisionSettings;
	collisionSettings.defaultVelocitySolverNbIterations = 5;
	collisionSettings.isSleepingEnabled = false;

	_zaWarudo = std::make_unique<rp3d::CollisionWorld>(collisionSettings);
}

physicsEngine::~physicsEngine()
{
}

void physicsEngine::notifyContact(const CollisionCallbackInfo& collisionCallbackInfo)
{
	entt::entity leftEntity = (entt::entity)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData();
	auto& velLeft = _enttRegistry->get<velocity>(leftEntity);
	auto bodyLeft = _enttRegistry->get<body>(leftEntity);
	double leftMass = bodyLeft.mass;
	entt::entity rightEntity = (entt::entity)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData();
	auto& velRight = _enttRegistry->get<velocity>(rightEntity);
	auto bodyRight = _enttRegistry->get<body>(rightEntity);
	double rightMass = bodyRight.mass;
	fdd oldRightVel = velRight.spd;
	velRight.spd = (((velLeft.spd * 2 * leftMass) - (velRight.spd * leftMass) + (velRight.spd * rightMass)) / (leftMass + rightMass));
	velLeft.spd = oldRightVel + velRight.spd - velLeft.spd;
	
	
	//std::cout << "collision detected"<<std::endl;
}

rp3d::CollisionWorld* physicsEngine::getWorld()
{
	return _zaWarudo.get();
}

void physicsEngine::setRegistry(entt::registry* reg)
{
	_enttRegistry = reg;
}
