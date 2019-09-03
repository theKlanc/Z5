#pragma once
#include <fdd.hpp>
#include <universeNode.hpp>

struct velocity {
	fdd spd;
};

void to_json(nlohmann::json& j, const velocity& n);
void from_json(const nlohmann::json& j, velocity& n);