#pragma once
#include "HardwareInterface/HardwareInterface.hpp"
#include "json.hpp"

struct drawable{
	std::string name;
	HI2::Texture* sprite;
};

void to_json(nlohmann::json& j, const drawable& n);
void from_json(const nlohmann::json& j, drawable& n);