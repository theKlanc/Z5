#pragma once
#include "reactPhysics3D/src/reactphysics3d.h"
#include "json.hpp"

struct body
{
	double height;
	double width;
	double mass;
	rp3d::RigidBody* collider;
	rp3d::CollisionShape* _collisionShape = nullptr;
	double elasticity = 0;
};

void to_json(nlohmann::json& j, const body& n);
void from_json(const nlohmann::json& j, body& n);