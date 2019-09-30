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

void physicsEngine::processCollisions(universeNode& universeBase, entt::registry& registry, double dt)
{
	_dt = dt;
	Services::physicsMutex.lock();
	{
		detectNodeNode(universeBase, dt);
		solveNodeNode(universeBase, dt);

		detectNodeEntity(universeBase, registry, dt);
		solveNodeEntity(universeBase, registry, dt);

		detectEntityEntity(registry, dt);
		solveEntityEntity(registry, dt);
	}
	Services::physicsMutex.unlock();
}

void physicsEngine::notifyContact(const CollisionCallbackInfo& collisionCallbackInfo)
{
	if (((collidedResponse*)collisionCallbackInfo.body1->getUserData())->type == ((collidedResponse*)collisionCallbackInfo.body2->getUserData())->type)
	{
		if (((collidedResponse*)collisionCallbackInfo.body1->getUserData())->type == ENTITY)
		{
			EntityEntityCallback(collisionCallbackInfo);
		}
		else
		{
			NodeNodeCallback(collisionCallbackInfo);
		}
	}
	else
	{
		NodeEntityCallback(collisionCallbackInfo);
	}

}

rp3d::CollisionWorld* physicsEngine::getWorld()
{
	return _zaWarudo.get();
}



void physicsEngine::detectNodeNode(universeNode& universe, double dt)
{
}

void physicsEngine::solveNodeNode(universeNode& universe, double dt)
{
}

void physicsEngine::detectNodeEntity(universeNode& universeBase, entt::registry& registry, double dt)
{
	auto bodyEntitiesView = registry.view<body>();
	for (const entt::entity& left : bodyEntitiesView) { //Update entities' positions
		position entityPos = registry.get<position>(left);

		std::vector<universeNode*> collidableNodes = entityPos.parent->getParent()->getChildren();
		for (universeNode* ntemp : entityPos.parent->getChildren())
		{
			collidableNodes.push_back(ntemp);
		}
		collidableNodes.push_back(entityPos.parent->getParent());

		for (universeNode* node : collidableNodes)
		{
			position pos = registry.get<position>(left);

			fdd posRelativeToNode = node->getLocalPos(pos.pos, pos.parent);
			collidedResponse cResponse;
			cResponse.type = NODE;
			collidedBody cBody;
			cBody.node = node;
			cResponse.body = cBody;

			rp3d::Vector3 entityPosition(posRelativeToNode.x, posRelativeToNode.y, posRelativeToNode.z);
			rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
			rp3d::Transform entityTransform(entityPosition, initOrientation);

			body& entityBody = registry.get<body>(left);
			entityBody.collider->setTransform(entityTransform);

			if (_zaWarudo->testAABBOverlap(entityBody.collider, node->getNodeCollider()))
			{
				auto chunksToCheck = node->getTerrainColliders(posRelativeToNode, node);
				for (auto& chunk : chunksToCheck)
				{
					chunk->setUserData((void*)& cResponse);
					_zaWarudo->testCollision(entityBody.collider, chunk, this);
				}
			}
		}
	}
}

void physicsEngine::solveNodeEntity(universeNode& universeBase, entt::registry& registry, double dt)
{
}

void physicsEngine::detectEntityEntity(entt::registry& registry, double dt)
{
	auto bodyEntitiesView = registry.view<body>();
	for (const entt::entity& left : bodyEntitiesView) { //Update entities' positions
		for (const entt::entity& right : bodyEntitiesView) { //Update entities' positions
			if (left > right)
			{

				position pL = registry.get<position>(left);
				position pR = registry.get<position>(right);
				fdd rightPos = pL.parent->getLocalPos(pR.pos, pR.parent);

				rp3d::Vector3 leftPosition(pL.pos.x, pL.pos.y, pL.pos.z);
				rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
				rp3d::Transform leftTransform(leftPosition, initOrientation);

				rp3d::Vector3 rightPosition(rightPos.x, rightPos.y, rightPos.z);
				rp3d::Transform rightTransform(rightPosition, initOrientation);

				body& leftBody = registry.get<body>(left);
				leftBody.collider->setTransform(leftTransform);
				body& rightBody = registry.get<body>(right);
				rightBody.collider->setTransform(rightTransform);
				if (_zaWarudo->testAABBOverlap(leftBody.collider, rightBody.collider))
				{
					_zaWarudo->testCollision(leftBody.collider, rightBody.collider, this);
				}
			}
		}
	}
}

void physicsEngine::solveEntityEntity(entt::registry& registry, double dt)
{
}


void physicsEngine::EntityEntityCallback(const CollisionCallbackInfo& collisionCallbackInfo) // solve collision between two entities in t
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
	
	if (positionLeft.pos.distance(positionRight.pos) < (positionLeft.pos + (velLeft.spd * _dt)).distance(positionRight.pos + (velRight.spd * _dt)))//s allunyaven
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

void physicsEngine::NodeEntityCallback(const CollisionCallbackInfo& collisionCallbackInfo)
{
	//std::cout << "Node-Entity collision" << std::endl;
	rp3d::Vector3 nodeShapePosition;
	universeNode* oldParent;
	entt::entity entity;
	universeNode* node;
	int entityBodyIndex = 0;
	rp3d::Vector3 entityContactNormal{ 0,0,0 };
	if (((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->type == ENTITY) { // BODY1 �s l entity
		entity = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.entity;
		entityBodyIndex = 1;
		node = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.node;
		nodeShapePosition = collisionCallbackInfo.proxyShape2->getLocalToWorldTransform().getPosition();
	}
	else
	{
		entity = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.entity;
		entityBodyIndex = 2;
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
		//avoid colliding with an object when getting away from it
		if (fdd{ nodeShapePosition.x,nodeShapePosition.y,nodeShapePosition.z,0 }.distance(entityPosition.pos) <= fdd{ nodeShapePosition.x,nodeShapePosition.y,nodeShapePosition.z,0 }.distance(entityPosition.pos + (entityVelocity.spd * _dt)))
		{
			return;
		}
	}

	auto& entityVel = Services::enttRegistry->get<velocity>(entity);

#pragma region solve position to surface
	fdd spd = entityVel.spd * _dt;
	double partialDepth = 0.5;
	double partialStep = 0.5;
	auto& entityPosition = Services::enttRegistry->get<position>(entity);

	for (int i = 0; i < 10; i++)
	{
		spd /= 2;
		partialStep /= 2;
		rp3d::CollisionBody* body1 = collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1();
		rp3d::CollisionBody* body2 = collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2();

		if (entityBodyIndex==1)
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
			entityPosition.pos -= spd;
			partialDepth += partialStep;
		}
		else
		{
			entityPosition.pos += spd;
			partialDepth -= partialStep;
		}
	}

	entityPosition.pos -= spd;


#pragma endregion

	//Calcular centroide de punts de contacte

	auto contactManifold = collisionCallbackInfo.contactManifoldElements->getContactManifold();
	int contactManifoldCount=0;
	while(contactManifold!=nullptr)
	{
		if (entityBodyIndex == 1)
			entityContactNormal += contactManifold->getContactPoints()->getLocalPointOnShape1() * -1;
		else
			entityContactNormal += contactManifold->getContactPoints()->getLocalPointOnShape2() * -1;
		contactManifold=contactManifold->getNext();
		contactManifoldCount++;
	}
	entityContactNormal /= contactManifoldCount;


	entityContactNormal.normalize();
	//Calculate new velocity
	rp3d::Vector3 d{ (rp3d::decimal)entityVel.spd.x,(rp3d::decimal)entityVel.spd.y,(rp3d::decimal)entityVel.spd.z };
	auto result = (d - (2 * (entityContactNormal.dot(d)) * entityContactNormal)) ;
	entityVel.spd.x = result.x;
	entityVel.spd.y = result.y;
	entityVel.spd.z = result.z;

	//apply new velocity from surface
	entityPosition.pos += entityVel.spd * _dt * (partialDepth)*0.5;

}

void physicsEngine::NodeNodeCallback(const CollisionCallbackInfo& collisionCallbackInfo)
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