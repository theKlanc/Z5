#pragma once
#include "reactPhysics3D/src/reactphysics3d.h"
#include "json.hpp"

struct body
{
	double height;
	double width;
	double mass;
	rp3d::CollisionBody* collider;
	rp3d::CollisionShape* _collisionShape = nullptr;
	double elasticity = 0;
	rp3d::Vector3 contactNormal;
	double maxContactDepth = 0;
	double volume;
};

void to_json(nlohmann::json& j, const body& n);
void from_json(const nlohmann::json& j, body& n);