#pragma once
#include "reactPhysics3D/src/reactphysics3d.h"
#include "json.hpp"

struct body
{
	double height;
	double width;
	double mass;
	rp3d::CollisionBody* collider; //We'll just use cylindrical colliders for now
	rp3d::CollisionShape* _collisionShape = nullptr;;
	void* lastCollided = nullptr;
};

void to_json(nlohmann::json& j, const body& n);
void from_json(const nlohmann::json& j, body& n);