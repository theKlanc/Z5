#pragma once
#include "HI2.hpp"
#include "json.hpp"
#include "graphicsManager.hpp"

struct drawable{
	std::string name;
	sprite* spr;
};

void to_json(nlohmann::json& j, const drawable& n);
void from_json(const nlohmann::json& j, drawable& n);