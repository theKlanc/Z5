#pragma once
#include "json.hpp"
#include "entt/entity/registry.hpp"

struct projectile
{
	projectile(unsigned dmg = 5) : _damage(dmg){};

	unsigned _damage = 5;
	int _remainingPenetration = 0;
	int remainingBounces = 1;

	std::optional<entt::entity> lastCollision;
};

void to_json(nlohmann::json& j, const projectile& n);
void from_json(const nlohmann::json& j, projectile& n);
