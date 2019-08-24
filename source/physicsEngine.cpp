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
	collisionSettings.worldName = "za warudo";

	_zaWarudo = std::make_unique<rp3d::CollisionWorld>(collisionSettings);
}

physicsEngine::~physicsEngine()
{}

void physicsEngine::notifyContact(const CollisionCallbackInfo& collisionCallbackInfo)
{
	if (((collidedResponse*)collisionCallbackInfo.body1->getUserData())->type == ((collidedResponse*)collisionCallbackInfo.body2->getUserData())->type)
	{
		if (((collidedResponse*)collisionCallbackInfo.body1->getUserData())->type == ENTITY)
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


void physicsEngine::solveEntityEntity(const CollisionCallbackInfo& collisionCallbackInfo) // solve collision between two entities in t
{
	entt::entity leftEntity = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.entity;
	auto& velLeft = Services::enttRegistry->get<velocity>(leftEntity);
	auto& positionLeft = Services::enttRegistry->get<position>(leftEntity);


	entt::entity rightEntity = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.entity;
	auto& velRight = Services::enttRegistry->get<velocity>(rightEntity);//falta passar posicio i velocitat de right al marc de referencia de left, i desfer al final
	auto positionRight = Services::enttRegistry->get<position>(rightEntity);
	positionLeft.pos = positionRight.parent->getLocalPos(positionLeft.pos, positionLeft.parent);

	velLeft.spd = positionRight.parent->getLocalVel(velLeft.spd, positionLeft.parent);
	positionLeft.parent = positionRight.parent;
	
	if (positionLeft.pos.distance(positionRight.pos) < (positionLeft.pos + (velLeft.spd * dt)).distance(positionRight.pos + (velRight.spd * dt)))//s allunyaven
	{
		return;
	}
	auto& bodyLeft = Services::enttRegistry->get<body>(leftEntity);
	double leftMass = bodyLeft.mass;
	auto& bodyRight = Services::enttRegistry->get<body>(rightEntity);
	double rightMass = bodyRight.mass;
	fdd oldRightVel = velRight.spd;
	velRight.spd = (((velLeft.spd * 2 * leftMass) - (velRight.spd * leftMass) + (velRight.spd * rightMass)) / (leftMass + rightMass)) * 0.95;
	velLeft.spd = (oldRightVel + velRight.spd - velLeft.spd) * 0.95;
}

void physicsEngine::solveNodeEntity(const CollisionCallbackInfo& collisionCallbackInfo)
{
	//std::cout << "Node-Entity collision" << std::endl;
	rp3d::Vector3 nodeShapePosition;
	universeNode* oldParent;
	entt::entity entity;
	universeNode* node;
	if (((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->type == ENTITY) { // BODY1 és l entity
		entity = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.entity;
		node = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.node;
		nodeShapePosition = collisionCallbackInfo.proxyShape2->getLocalToWorldTransform().getPosition();
	}
	else
	{
		entity = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.entity;
		node = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.node;
		nodeShapePosition = collisionCallbackInfo.proxyShape1->getLocalToWorldTransform().getPosition();
	}

	{//convert to local
		position& pos = Services::enttRegistry->get<position>(entity);
		oldParent = pos.parent;
		pos.pos = node->getLocalPos(pos.pos, oldParent);
		pos.parent = node;

		velocity& vel = Services::enttRegistry->get<velocity>(entity);
		vel.spd = node->getLocalVel(vel.spd, oldParent);
	}

	{
		auto entityPosition = Services::enttRegistry->get<position>(entity);
		auto entityVelocity = Services::enttRegistry->get<velocity>(entity);
		if (fdd{ nodeShapePosition.x,nodeShapePosition.y,nodeShapePosition.z,0 }.distance(entityPosition.pos) < fdd{ nodeShapePosition.x,nodeShapePosition.y,nodeShapePosition.z,0 }.distance(entityPosition.pos + (entityVelocity.spd * dt)))
		{
			return;
		}
	}

	auto& entityBody = Services::enttRegistry->get<body>(entity);
	if (entityBody.lastCollided == node)
	{
		return;
	}
	else
	{
		entityBody.lastCollided = node;
	}

	auto& entityVel = Services::enttRegistry->get<velocity>(entity);

#pragma region solve position
	fdd spd = entityVel.spd / 2;

	auto& entityPosition = Services::enttRegistry->get<position>(entity);
	entityPosition.pos += entityVel.spd * spd * dt;
	for (int i = 0; i < 10; i++)
	{
		spd /= 2;
		rp3d::CollisionBody* body1 = collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1();
		rp3d::CollisionBody* body2 = collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2();

		if (((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->type == ENTITY)
		{
			auto t = body1->getTransform();
			body1->setTransform({ {(rp3d::decimal)entityPosition.pos.x,(rp3d::decimal)entityPosition.pos.y,(rp3d::decimal)entityPosition.pos.z},rp3d::Quaternion::identity() });
		}
		else
		{
			auto t = body2->getTransform();
			body2->setTransform({ {(rp3d::decimal)entityPosition.pos.x,(rp3d::decimal)entityPosition.pos.y,(rp3d::decimal)entityPosition.pos.z},rp3d::Quaternion::identity() });
		}
		if (_zaWarudo->testOverlap(body1, body2))
		{
			entityPosition.pos += entityVel.spd * spd * dt;
		}
		else
		{
			entityPosition.pos -= entityVel.spd * spd * dt;
		}
	}

	entityPosition.pos += (entityVel.spd / entityVel.spd.distance({ 0,0,0,0 })) * spd * 2 * dt;

#pragma endregion

	//Calculate new velocity
	auto vector = collisionCallbackInfo.contactManifoldElements->getContactManifold()->getContactPoints()->getNormal();
	if (abs(vector.x) > abs(vector.y) && abs(vector.x) > abs(vector.z))
	{
		entityVel.spd.x *= -0.5;
	}
	else if (abs(vector.y) > abs(vector.z) && abs(vector.y) > abs(vector.z))
	{
		entityVel.spd.y *= -0.5;
	}
	else
	{
		entityVel.spd.z *= -0.5;
	}
}

void physicsEngine::solveNodeNode(const CollisionCallbackInfo& collisionCallbackInfo)
{
	universeNode* left = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.node;
	double leftMass = left->getMass();
	universeNode* right = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.node;
	double rightMass = right->getMass();
	fdd oldRightVel = left->getLocalVel(right->getVelocity(), right->getParent());
	fdd newRightVel = (((left->getVelocity() * 2 * leftMass) - (oldRightVel * leftMass) + (oldRightVel * rightMass)) / (leftMass + rightMass)) * 0.95;
	left->setVelocity((oldRightVel + newRightVel - left->getVelocity()) * 0.95);
	right->setVelocity(right->getLocalVel(newRightVel, left->getParent()));
}
