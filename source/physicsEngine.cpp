#include "physicsEngine.hpp"
#include <iostream>
#include "components/name.hpp"
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
	_remainingTime += dt;
	if (_remainingTime > 2.0f)
		_remainingTime = 2.0f;

	Services::physicsMutex.lock();
	{
		while (_remainingTime > _timeStep) {
			if (config::gravityEnabled)
			{
				applyGravity(universeBase, registry, _timeStep);
				applyBuoyancy(universeBase, registry, _timeStep);
			}

			if (config::dragEnabled)
			{
				applyDrag(universeBase, registry, _timeStep);
			}

			universeBase.updatePositions(_timeStep);

			detectNodeNode(universeBase, _timeStep);
			solveNodeNode(universeBase, _timeStep);

			{
				for (int i = 0; i < config::physicsSolverIterations; ++i)
				{
					applyVelocity(universeBase, registry, _solverStep);

					detectNodeEntity(universeBase, registry, _solverStep);
					solveNodeEntity(universeBase, registry, _solverStep);
				}
				detectEntityEntity(registry, _timeStep);
				solveEntityEntity(registry, _timeStep);
			}
			_remainingTime -= _timeStep;
		}
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

rp3d::CollisionWorld* physicsEngine::getWorld() const
{
	return _zaWarudo.get();
}

void physicsEngine::applyGravity(universeNode& universeBase, entt::registry& registry, double dt)
{
	for (universeNode& node : universeBase) {
		if (node.getParent() != nullptr && !node.physicsData.sleeping)
		{
			node.setVelocity(node.getVelocity() + node.getParent()->getGravityAcceleration(node.getPosition() + node.getCenterOfMass()) * dt);
			assert(!std::isnan(node.getVelocity().x));
		}
	}
	auto movableEntityView = registry.view<velocity, position>();
	for (const entt::entity& entity : movableEntityView) {
		velocity& vel = movableEntityView.get<velocity>(entity);
		const position& pos = movableEntityView.get<position>(entity);

		vel.spd += (pos.parent->getGravityAcceleration(pos.pos) * dt);
	}
}

void physicsEngine::applyBuoyancy(universeNode& universeBase, entt::registry& registry, double dt)
{
	for (universeNode& node : universeBase) {
		if (node.getParent() != nullptr && !node.physicsData.sleeping)
		{
			fdd pos = node.getCenterOfMass();
			pos+=node.getPosition();
			metaBlock block = node.getParent()->getBlock({ (int)floor(pos.x),(int)floor(pos.y),(int)floor(pos.z) });
			if (block.base->ID == 0 || block.base->solid)
				continue;
			fdd buoyancy = node.getParent()->getGravityAcceleration(pos) * -1 * ((node.getDiameter() / 2) * (node.getDiameter() / 2) * 4 * M_PI) * block.base->mass;
			//Bforce = p_fluidDensity * V * g_gravityAcceleration
			node.setVelocity(node.getVelocity() + (buoyancy / node.getMass()) * dt);
			assert(!std::isnan(node.getVelocity().x));
		}
	}
	auto movableEntityView = registry.view<velocity, position, body>();
	for (const entt::entity& entity : movableEntityView) {
		velocity& vel = movableEntityView.get<velocity>(entity);
		const position& pos = movableEntityView.get<position>(entity);
		const body& bdy = movableEntityView.get<body>(entity);

		metaBlock block = pos.parent->getBlock({ (int)floor(pos.pos.x),(int)floor(pos.pos.y),(int)floor(pos.pos.z + bdy.height / 2) });
		if (block.base->ID == 0 || block.base->solid)
			continue;
		fdd buoyancy = pos.parent->getGravityAcceleration(pos.pos) * -1 * (bdy.volume) * block.base->mass;
		//Bforce = p_fluidDensity * V * g_gravityAcceleration
		vel.spd += (buoyancy / bdy.mass) * dt;
	}
}

void physicsEngine::applyDrag(universeNode& universeBase, entt::registry& registry, double dt)
{
	for (universeNode& node : universeBase) {
		if (node.getParent() != nullptr)
		{
			fdd pos = node.getCenterOfMass();
			pos+=node.getPosition();
			fdd vel = node.getVelocity();
			metaBlock block = node.getParent()->getBlock({ (int)floor(pos.x),(int)floor(pos.y),(int)floor(pos.z) });
			if (block.base->ID == 0)
				continue;
			fdd drag = (vel * vel) * block.base->mass * (sqrt((node.getDiameter() / 2) * (node.getDiameter() / 2) * 4 * M_PI)) * 0.25;
			if ((drag.x > 0 && vel.x < 0) || (drag.x < 0 && vel.x>0))
				drag.x *= -1;
			if ((drag.y > 0 && vel.y < 0) || (drag.y < 0 && vel.y>0))
				drag.y *= -1;
			if ((drag.z > 0 && vel.z < 0) || (drag.z < 0 && vel.z>0))
				drag.z *= -1;
			if ((drag.r > 0 && vel.r < 0) || (drag.r < 0 && vel.r>0))
				drag.r *= -1;
			drag.r /= 2;
			drag *= -1;
			node.setVelocity(node.getVelocity() + (drag / node.getMass()) * dt);
			assert(!std::isnan(node.getVelocity().x));
		}
	}
	auto movableEntityView = registry.view<velocity, position, body>();
	for (const entt::entity& entity : movableEntityView) {
		velocity& vel = movableEntityView.get<velocity>(entity);
		const position& pos = movableEntityView.get<position>(entity);
		const body& bdy = movableEntityView.get<body>(entity);
		//F=(1/2)*(densityOfFluid)*(velocity^2)*(Area)*(DragCoefficient)
		metaBlock block = pos.parent->getBlock({ (int)floor(pos.pos.x),(int)floor(pos.pos.y),(int)floor(pos.pos.z + bdy.height / 2) });
		if (block.base->ID == 0)
			continue;
		fdd drag = (vel.spd * vel.spd) * block.base->mass * (sqrt(bdy.volume)) * 0.25;
		if ((drag.x > 0 && vel.spd.x < 0) || (drag.x < 0 && vel.spd.x>0))
			drag.x *= -1;
		if ((drag.y > 0 && vel.spd.y < 0) || (drag.y < 0 && vel.spd.y>0))
			drag.y *= -1;
		if ((drag.z > 0 && vel.spd.z < 0) || (drag.z < 0 && vel.spd.z>0))
			drag.z *= -1;
		if ((drag.r > 0 && vel.spd.r < 0) || (drag.r < 0 && vel.spd.r>0))
			drag.r *= -1;
		drag.r /= 2;
		drag *= -1;
		vel.spd += (drag / bdy.mass) * dt;
	}
}


void physicsEngine::applyVelocity(universeNode& universeBase, entt::registry& registry, double dt)
{
	auto movableEntityView = registry.view<velocity, position>();
	for (const entt::entity& entity : movableEntityView) { //Update entities' positions
		const velocity& vel = movableEntityView.get<velocity>(entity);
		position& pos = movableEntityView.get<position>(entity);
		pos.pos += (vel.spd * dt);
		pos.pos.r -= floor(pos.pos.r / (2 * M_PI)) * 2 * M_PI;
		if (std::isnan(pos.pos.r))
			pos.pos.r = 0;
	}
}

void physicsEngine::testCollisionBetweenNodes(universeNode& left, universeNode& right)
{
	fdd transformRightToLeft = right.getLocalPos(left.getPosition(), left.getParent());
	for (terrainChunk& leftChunk : left.getChunks()) {
		if (leftChunk.loaded())
		{
			auto rightChunks = right.getCollidableChunks(left.getPosition() + leftChunk.getPosition(), { config::chunkSize,config::chunkSize,config::chunkSize }, left.getParent());
			{
				fdd tmp = transformRightToLeft + leftChunk.getPosition();
				rp3d::Vector3 leftChunkPosVector(tmp.x, tmp.y, tmp.z);
				rp3d::Transform leftChunkTransform(leftChunkPosVector, rp3d::Quaternion::identity());

				leftChunk.getCollider()->setTransform(leftChunkTransform);
			}
			for (terrainChunk* rightChunk : rightChunks)
			{
				if (rightChunk->loaded())
				{
					rp3d::Vector3 rightChunkPosVector(rightChunk->getPosition().x, rightChunk->getPosition().y, rightChunk->getPosition().z);
					rp3d::Transform rightChunkTransform(rightChunkPosVector, rp3d::Quaternion::identity());

					rightChunk->getCollider()->setTransform(rightChunkTransform);


					if (_zaWarudo->testAABBOverlap(rightChunk->getCollider(), leftChunk.getCollider()))
					{
						collidedResponse cResponse;
						cResponse.type = NODE;
						cResponse.body.node = &left;

						leftChunk.getCollider()->setUserData(&cResponse);

						collidedResponse cResponse2;
						cResponse2.type = NODE;
						cResponse2.body.node = &right;

						rightChunk->getCollider()->setUserData(&cResponse2);
						_zaWarudo->testCollision(leftChunk.getCollider(), rightChunk->getCollider(), this);
					}
				}
			}
		}
	}
}

void physicsEngine::detectNodeNode(universeNode& universe, double dt)
{
	for (universeNode& node : universe) {
		node.physicsData.maxContactDepth = 0;
		node.physicsData.contactNormal = rp3d::Vector3();

		if (node.getParent() != nullptr && !node.physicsData.sleeping)
		{
			node.getNodeCollider()->setTransform({ node.getPosition().getVector3(),rp3d::Quaternion::identity() });



			if (node.getNodeCollider()->testAABBOverlap(node.getParent()->getNodeCollider()->getAABB()))
			{
				//parent
				testCollisionBetweenNodes(node, *node.getParent());
			}


			//brethren
			for (universeNode* brotha : universe.getChildren())
			{
				if (*brotha != node)
				{
					if (brotha->physicsData.sleeping || brotha->getID() < node.getID())
					{
						brotha->getNodeCollider()->setTransform({ brotha->getPosition().getVector3(),rp3d::Quaternion::identity() });
						if (node.getNodeCollider()->testAABBOverlap(node.getParent()->getNodeCollider()->getAABB()))
						{
							//TODO
						}
					}
				}
			}
			node.getNodeCollider()->setTransform({ rp3d::Vector3::zero(),rp3d::Quaternion::identity() });
		}
	}
}



void physicsEngine::detectNodeEntity(universeNode& universeBase, entt::registry& registry, double dt)
{
	auto bodyEntitiesView = registry.view<body>();
	for (const entt::entity& left : bodyEntitiesView) { //Update entities' positions
		position entityPos = registry.get<position>(left);
		body& entityBody = registry.get<body>(left);

		collidedResponse cResponse2;
		cResponse2.type = ENTITY;
		cResponse2.body.entity = left;

		entityBody.physicsData.collider->setUserData(&cResponse2);

		entityBody.physicsData.maxContactDepth = 0;
		entityBody.physicsData.contactNormal = rp3d::Vector3();

		std::vector<universeNode*> collidableNodes;
		if(entityPos.parent->getParent() != nullptr){
			collidableNodes = entityPos.parent->getParent()->getChildren();
			for (universeNode* ntemp : entityPos.parent->getChildren())
			{
				collidableNodes.push_back(ntemp);
			}
				collidableNodes.push_back(entityPos.parent->getParent());
		}
		else{
			collidableNodes.push_back(entityPos.parent);
		}

		for (universeNode* node : collidableNodes)
		{
			position pos = registry.get<position>(left);
			//if(std::isnan(pos.pos.x)){
			//	std::cout << "Resetting position of entity " << registry.get<name>(left).nameString << " due to nan" << std::endl;
			//	registry.get<position>(left).pos = fdd();
			//	continue;
			//}

 			fdd posRelativeToNode = node->getLocalPos(pos.pos, pos.parent);

			collidedResponse cResponse;
			cResponse.type = NODE;
			cResponse.body.node = node;

			rp3d::Vector3 entityPosition(posRelativeToNode.x, posRelativeToNode.y, posRelativeToNode.z);
			rp3d::Transform entityTransform(entityPosition, rp3d::Quaternion::identity());


			entityBody.physicsData.collider->setTransform(entityTransform);

			if (_zaWarudo->testAABBOverlap(entityBody.physicsData.collider, node->getNodeCollider()))
			{
				auto chunksToCheck = node->getCollidableChunks({ posRelativeToNode.x - entityBody.width / 2,
																posRelativeToNode.y - entityBody.width / 2,
																posRelativeToNode.z },
					point3Dd{ entityBody.width,entityBody.width,entityBody.width },
					node);

				for (terrainChunk*& chunk : chunksToCheck)
				{
					fdd posRelativeToChunk = node->getLocalPos(pos.pos, pos.parent) - chunk->getPosition();
					entityTransform.setPosition(posRelativeToChunk.getVector3());
					entityBody.physicsData.collider->setTransform(entityTransform);

					chunk->getCollider()->setTransform(rp3d::Transform::identity());

					if (chunk->getCollider()->testAABBOverlap(entityBody.physicsData.collider->getAABB()));
					{
						chunk->getCollider()->setUserData((void*)&cResponse);
						_zaWarudo->testCollision(entityBody.physicsData.collider, chunk->getCollider(), this);
					}
				}
			}
		}
	}
}

void physicsEngine::solveNodeEntity(universeNode& universeBase, entt::registry& registry, double dt)
{
	auto bodyView = registry.view<body, position, velocity>();
	for (const entt::entity& entity : bodyView)
	{
		body& bdy = bodyView.get<body>(entity);
		if (bdy.physicsData.maxContactDepth > 0.0001)
		{
			position& pos = bodyView.get<position>(entity);
			velocity& vel = bodyView.get<velocity>(entity);
			fdd oldPos = pos.pos;
			fdd oldSpeed = vel.spd;
			//backtrack position along the old ~velocity~  until we're "just" colliding with the node (tangentially)
			//pos.pos -= vel.spd * dt * ((bdy.physicsData.maxContactDepth) / ((oldSpeed * dt).magnitude()));
			pos.pos -= bdy.physicsData.contactNormal * bdy.physicsData.maxContactDepth;
			//Calculate new velocity
			rp3d::Vector3 d{ (rp3d::decimal)vel.spd.x,(rp3d::decimal)vel.spd.y,(rp3d::decimal)vel.spd.z };
			auto result = (d - (2 * (bdy.physicsData.contactNormal.dot(d)) * bdy.physicsData.contactNormal));

			fdd normal;
			normal.x = bdy.physicsData.contactNormal.x;
			normal.y = bdy.physicsData.contactNormal.y;
			normal.z = bdy.physicsData.contactNormal.z;

			fdd projectedFriction = normal.project(vel.spd);
			projectedFriction *= (1 - bdy.elasticity);

			result.x += projectedFriction.x;
			result.y += projectedFriction.y;
			result.z += projectedFriction.z;

			vel.spd.x = result.x;
			vel.spd.y = result.y;
			vel.spd.z = result.z;

			//apply new velocity from surface
			pos.pos += vel.spd * dt * (bdy.physicsData.maxContactDepth / ((oldSpeed * dt).magnitude()));
			if(std::isnan(pos.pos.x)){
				std::cout << "OldVel: " << oldSpeed << std::endl;
				std::cout << "OldPos: " << oldPos << std::endl;
				std::cout << "resultat: " << result << std::endl;
				std::cout << "contactNormal: " << bdy.physicsData.contactNormal << std::endl;
				std::cout << "contactDepth: " << bdy.physicsData.maxContactDepth << std::endl;
				std::cout << "dt: " << dt << std::endl;
			}
			assert(!std::isnan(pos.pos.x));
		}
	}
}

void physicsEngine::solveNodeNode(universeNode& universe, double dt)
{
	for (universeNode& node : universe)
	{
		if (node.physicsData.maxContactDepth == 0)
			continue;
		fdd oldSpeed = node.getVelocity();
		fdd pos = node.getPosition();
		fdd vel = node.getVelocity();
		pos -= node.physicsData.contactNormal * node.physicsData.maxContactDepth;

		rp3d::Vector3 d{ (rp3d::decimal)vel.x,(rp3d::decimal)vel.y,(rp3d::decimal)vel.z };
		auto result = (d - (2 * (node.physicsData.contactNormal.dot(d)) * node.physicsData.contactNormal));

		fdd normal;
		normal.x = node.physicsData.contactNormal.x;
		normal.y = node.physicsData.contactNormal.y;
		normal.z = node.physicsData.contactNormal.z;

		fdd projectedFriction = normal.project(vel);
		projectedFriction *= 0.5;

		result.x += projectedFriction.x;
		result.y += projectedFriction.y;
		result.z += projectedFriction.z;

		vel.x = result.x;
		vel.y = result.y;
		vel.z = result.z;

		//apply new velocity from surface
		pos += vel * dt * (node.physicsData.maxContactDepth / ((oldSpeed * dt).magnitude())) * 0.8;

		assert(!std::isnan(pos.x));
		assert(!std::isnan(vel.x));
		node.setPosition(pos);
		node.setVelocity(vel);

		if (vel.magnitude() < 0.1)
		{
			node.physicsData.sleeping = true;
			std::cout << "putting " << node.getName() << " to sleep" << std::endl;
		}
	}
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
				leftBody.physicsData.collider->setTransform(leftTransform);

				collidedResponse cResponse;
				cResponse.type = ENTITY;
				cResponse.body.entity = left;

				leftBody.physicsData.collider->setUserData(&cResponse);

				body& rightBody = registry.get<body>(right);
				rightBody.physicsData.collider->setTransform(rightTransform);

				collidedResponse cResponse2;
				cResponse2.type = ENTITY;
				cResponse2.body.entity = right;

				rightBody.physicsData.collider->setUserData(&cResponse2);

				_zaWarudo->testCollision(leftBody.physicsData.collider, rightBody.physicsData.collider, this);
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

	if (positionLeft.pos.distance(positionRight.pos) < (positionLeft.pos + (velLeft.spd * _solverStep)).distance(positionRight.pos + (velRight.spd * _solverStep)))//s allunyaven
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
	universeNode* oldParent;
	entt::entity entity;
	universeNode* node;
	rp3d::CollisionBody* entityCollisionBody;
	if (((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->type == ENTITY) { // BODY1 �s l entity
		entityCollisionBody = collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1();
		node = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.node;
	}
	else
	{
		entityCollisionBody = collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2();
		node = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.node;
	}
	entity = ((collidedResponse*)entityCollisionBody->getUserData())->body.entity;

	position& pos = Services::enttRegistry->get<position>(entity);
	body& entityBody = Services::enttRegistry->get<body>(entity);
	auto contactManifold = collisionCallbackInfo.contactManifoldElements->getContactManifold();
	while (contactManifold != nullptr)
	{
		auto contactPoint = contactManifold->getContactPoints();
		while (contactPoint != nullptr)
		{
			if (entityBody.physicsData.maxContactDepth < contactPoint->getPenetrationDepth())
			{
				if (pos.parent != node)
				{//convert to local
				//TODO avoid this if possible

					oldParent = pos.parent;
					pos.pos = node->getLocalPos(pos.pos, oldParent);
					pos.parent = node;

					velocity& vel = Services::enttRegistry->get<velocity>(entity);
					vel.spd = node->getLocalVel(vel.spd, oldParent);
				}

				entityBody.physicsData.maxContactDepth = contactPoint->getPenetrationDepth();
				entityBody.physicsData.contactNormal = contactPoint->getNormal();
			}
			contactPoint = contactPoint->getNext();
		}
		contactManifold = contactManifold->getNext();
	}
}

void physicsEngine::NodeNodeCallback(const CollisionCallbackInfo& collisionCallbackInfo)
{//TODO correctly handle cases where mass difference is not immense
	universeNode* fatNode;
	universeNode* slimNode;
	bool invertNormal = false;
	if (((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.node->getMass() > ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.node->getMass())
	{
		fatNode = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.node;
		slimNode = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.node;

		invertNormal = true;
	}
	else
	{
		fatNode = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody2()->getUserData())->body.node;
		slimNode = ((collidedResponse*)collisionCallbackInfo.contactManifoldElements->getContactManifold()->getBody1()->getUserData())->body.node;
	}

	auto contactManifold = collisionCallbackInfo.contactManifoldElements->getContactManifold();
	while (contactManifold != nullptr)
	{
		auto contactPoint = contactManifold->getContactPoints();
		while (contactPoint != nullptr)
		{
			if (slimNode->physicsData.maxContactDepth < contactPoint->getPenetrationDepth())
			{
				slimNode->physicsData.maxContactDepth = contactPoint->getPenetrationDepth();
				slimNode->physicsData.contactNormal = contactPoint->getNormal();
				if (invertNormal)
					slimNode->physicsData.contactNormal *= -1;
			}
			contactPoint = contactPoint->getNext();
		}
		contactManifold = contactManifold->getNext();
	}
}