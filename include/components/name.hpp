#pragma once
#include <string>
#include "json.hpp"

struct name
{
	std::string nameString;
};

void to_json(nlohmann::json& j, const name& n);
void from_json(const nlohmann::json& j, name& n);