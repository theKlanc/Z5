#pragma once
#include "json.hpp"

struct projectile
{
	projectile(unsigned dmg = 5) : _damage(dmg){};

	unsigned _damage;
};

void to_json(nlohmann::json& j, const projectile& n);
void from_json(const nlohmann::json& j, projectile& n);
