#include "physicsEngine.hpp"
#include <iostream>
#include "reactPhysics3D/src/reactphysics3d.h"
#include "reactPhysics3D/src/collision/ContactManifold.h"
#include "reactPhysics3D/src/constraint/ContactPoint.h"
#include "entt/entity/fwd.hpp"
#include "components/velocity.hpp"
#include "components/body.hpp"
#include "components/position.hpp"
#include "entt/entity/helper.hpp"


physicsEngine::physicsEngine()
{
	//Create collision world
	rp3d::WorldSettings worldSettings;
	//collisionSettings.defaultVelocitySolverNbIterations = 5;
	//collisionSettings.isSleepingEnabled = false;
	worldSettings.worldName = "za warudo";
	worldSettings.defaultBounciness = 0.5f;
	rp3d::Vector3 gravityVector(0, 0, 0);

	_zaWarudo = std::make_unique<rp3d::DynamicsWorld>(gravityVector, worldSettings);
}

rp3d::DynamicsWorld* physicsEngine::getWorld()
{
	return _zaWarudo.get();
}

void physicsEngine::processCollisions(universeNode& universeBase, entt::registry& registry, double dt)
{
	_dt = dt;
	Services::physicsMutex.lock();
	{
		//preproces, activem els nodes i entitats a colisionar, activem i transformem posicions i velocitats relatives a la camera

		auto cameraView = registry.view<entt::tag<"CAMERA"_hs>>();
		entt::entity camera;
		for (auto& c : cameraView)
		{
			camera = c;
		}
		position cameraPos = registry.get<position>(camera);

		std::vector<universeNode*> universeNodes = universeBase.getFlattenedTree();
		{	//Actualitzem velocitats i posicions dels nodes, després entitats
			for (universeNode* u : universeNodes)
			{
				fdd	uPosOnCamera = cameraPos.parent->getLocalPos(u->getPosition(), u->getParent() == nullptr ? u : u->getParent());
				fdd	uVelOnCamera = cameraPos.parent->getLocalVel(u->getVelocity(), u->getParent() == nullptr ? u : u->getParent());

				u->getNodeCollider()->setTransform(rp3d::Transform{ {static_cast<reactphysics3d::decimal>(uPosOnCamera.x),
					static_cast<reactphysics3d::decimal>(uPosOnCamera.y), static_cast<reactphysics3d::decimal>(uPosOnCamera.z)
				},rp3d::Quaternion::identity() });
				u->getNodeCollider()->setLinearVelocity(rp3d::Vector3{ static_cast<reactphysics3d::decimal>(uVelOnCamera.x),
					static_cast<reactphysics3d::decimal>(uVelOnCamera.y), static_cast<reactphysics3d::decimal>(uVelOnCamera.z)
					});//TODO rotation
			}
			auto positionView = registry.view<position, body>();
			for (entt::entity e : positionView)
			{
				body& b = registry.get<body>(e);
				position& p = registry.get<position>(e);
				fdd	ePosOnCamera = cameraPos.parent->getLocalPos(p.pos, p.parent);
				b.collider->setTransform(rp3d::Transform{ {static_cast<reactphysics3d::decimal>(ePosOnCamera.x),
					static_cast<reactphysics3d::decimal>(ePosOnCamera.y), static_cast<reactphysics3d::decimal>(ePosOnCamera.z)
				},rp3d::Quaternion::identity() });
				if (registry.has<velocity>(e))
				{
					velocity& v = registry.get<velocity>(e);
					fdd	eVelOnCamera = cameraPos.parent->getLocalVel(v.spd, p.parent);
					b.collider->setLinearVelocity(rp3d::Vector3{ static_cast<reactphysics3d::decimal>(ePosOnCamera.x),
						static_cast<reactphysics3d::decimal>(ePosOnCamera.y), static_cast<reactphysics3d::decimal>(ePosOnCamera.z)
						});
				}
			}
		}

		//calcular ticks
		_remainingTime += dt;
		while (_remainingTime > _physicsStep)
		{
			_remainingTime -= _physicsStep;
			_zaWarudo.get()->update(_physicsStep);
		}

		//postprocés, tornem posicions i velocitats relatives al seu parent
		for (universeNode*& u : universeNodes)
		{

			rp3d::Transform uPosOnCamera = u->getNodeCollider()->getTransform();
			rp3d::Vector3 uVelOnCamera = u->getNodeCollider()->getLinearVelocity();
			if (u->getParent() == nullptr)
			{
				u->setPosition(u->getLocalPos(fdd{ uPosOnCamera.getPosition().x,uPosOnCamera.getPosition().y,uPosOnCamera.getPosition().z }, cameraPos.parent));//TODO rotation
				u->setVelocity(u->getLocalVel(fdd{ uVelOnCamera.x,uVelOnCamera.y,uVelOnCamera.z }, cameraPos.parent));//TODO rotation
			}
			else {
				u->setPosition(u->getParent()->getLocalPos(fdd{ uPosOnCamera.getPosition().x,uPosOnCamera.getPosition().y,uPosOnCamera.getPosition().z }, cameraPos.parent));//TODO rotation
				u->setVelocity(u->getParent()->getLocalVel(fdd{ uVelOnCamera.x,uVelOnCamera.y,uVelOnCamera.z }, cameraPos.parent));//TODO rotation
			}

		}

		auto positionView = registry.view<position, body>();
		for (entt::entity e : positionView)
		{
			body& b = registry.get<body>(e);
			position& p = registry.get<position>(e);
			rp3d::Transform	ePosOnCamera = b.collider->getTransform();
			p.pos = p.parent->getLocalPos(fdd{ ePosOnCamera.getPosition().x,ePosOnCamera.getPosition().y,ePosOnCamera.getPosition().z }, cameraPos.parent);
			if (registry.has<velocity>(e))
			{
				velocity& v = registry.get<velocity>(e);
				rp3d::Vector3 eVelOnCamera = b.collider->getLinearVelocity();
				v.spd = p.parent->getLocalVel(fdd{ eVelOnCamera.x,eVelOnCamera.y,eVelOnCamera.z }, cameraPos.parent);
			}
		}
	}
	Services::physicsMutex.unlock();
}
