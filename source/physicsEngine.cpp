#pragma once
#include "physicsEngine.hpp"
#include <iostream>
#include "reactPhysics3D/src/reactphysics3d.h"
#include "reactPhysics3D/src/collision/ContactManifold.h"
#include "reactPhysics3D/src/constraint/ContactPoint.h"
#include "entt/entity/fwd.hpp"
#include "components/velocity.hpp"
#include "components/body.hpp"
#include "components/position.hpp"


physicsEngine::physicsEngine()
{
	//Create collision world
	rp3d::WorldSettings collisionSettings;
	collisionSettings.defaultVelocitySolverNbIterations = 5;
	collisionSettings.isSleepingEnabled = false;
	collisionSettings.worldName="za warudo";

	_zaWarudo = std::make_unique<rp3d::CollisionWorld>(collisionSettings);
}

physicsEngine::~physicsEngine()
{}

void physicsEngine::notifyContact(const CollisionCallbackInfo& collisionCallbackInfo)
{
	if(((collidedResponse*)collisionCallbackInfo.body1->getUserData())->type == ((collidedResponse*)collisionCallbackInfo.body2->getUserData())->type)
	{
		if(((collidedResponse*)collisionCallbackInfo.body1->getUserData())->type==ENTITY)
		{
			solveEntityEntity(collisionCallbackInfo);
		}
		else
		{
			solveNodeNode(collisionCallbackInfo);
		}
	}
	else
	{
		solveNodeEntity(collisionCallbackInfo);
	}
	
}

rp3d::CollisionWorld* physicsEngine::getWorld()
{
	return _zaWarudo.get();
}


void physicsEngine::solveEntityEntity(const CollisionCallbackInfo& collisionCallbackInfo)
{
	entt::entity leftEntity = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.entity;
	auto& velLeft = Services::enttRegistry->get<velocity>(leftEntity);
	auto positionLeft = Services::enttRegistry->get<position>(leftEntity);
	
	entt::entity rightEntity = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.entity;
	auto& velRight = Services::enttRegistry->get<velocity>(rightEntity);
	auto positionRight = Services::enttRegistry->get<position>(rightEntity);
	if(positionLeft.pos.distance(positionRight.pos) < (positionLeft.pos + (velLeft.spd*dt)).distance(positionRight.pos+(velRight.spd*dt)))//s allunyaven
	{
		return;
	}
	auto& bodyLeft = Services::enttRegistry->get<body>(leftEntity);
	double leftMass = bodyLeft.mass;
	auto& bodyRight = Services::enttRegistry->get<body>(rightEntity);
	double rightMass = bodyRight.mass;
	fdd oldRightVel = velRight.spd;
	velRight.spd = (((velLeft.spd * 2 * leftMass) - (velRight.spd * leftMass) + (velRight.spd * rightMass)) / (leftMass + rightMass))*0.95;
	//velRight.spd.r*=-1;
	velLeft.spd = (oldRightVel + velRight.spd - velLeft.spd)*0.95;
	//velLeft.spd.r*=-1;
}

void physicsEngine::solveNodeEntity(const CollisionCallbackInfo& collisionCallbackInfo)
{
	entt::entity entity;
	if(((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->type==entity){
		entity = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.entity;
	}
	else
	{
		entity = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.entity;
	}
	auto& vel = Services::enttRegistry->get<velocity>(entity);
	auto vector = collisionCallbackInfo.contactManifoldElements->getContactManifold()->getContactPoints()->getNormal();
	if(abs(vector.x) > abs(vector.y) && abs(vector.x) > abs(vector.z))
	{
		vel.spd.x *= -0.95;
	}
	else if (abs(vector.y) > abs(vector.z) && abs(vector.y) > abs(vector.z))
	{
		vel.spd.y *= -0.95;
	}
	else
	{
		vel.spd.z *= -0.95;
	}
}

void physicsEngine::solveNodeNode(const CollisionCallbackInfo& collisionCallbackInfo)
{
	universeNode* left = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.node;
	double leftMass = left->getMass();
	universeNode* right = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.node;
	double rightMass = right->getMass();
	fdd oldRightVel = left->getLocalFdd(right->getVelocity(),right->getParent());
	fdd newRightVel = (((left->getVelocity() * 2 * leftMass) - (oldRightVel * leftMass) + (oldRightVel * rightMass)) / (leftMass + rightMass)) * 0.95;
	left->setVelocity((oldRightVel + newRightVel - left->getVelocity()) * 0.95);
	right->setVelocity(right->getLocalFdd(newRightVel, left->getParent()));
}
