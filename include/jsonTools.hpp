#pragma once
#include <vector>
#include <string>
#include "json.hpp"
#include "entt/entity/fwd.hpp"
#include "HI2.hpp"

enum componentType{
	NAME,
	POSITION,
	VELOCITY,
	BODY,
	DRAWABLE,
	BRAIN,
};

NLOHMANN_JSON_SERIALIZE_ENUM( componentType, {
	{NAME,"NAME"},
	{POSITION,"POSITION"},
	{VELOCITY,"VELOCITY"},
	{BODY,"BODY"},
	{DRAWABLE,"DRAWABLE"},
	{BRAIN,"BRAIN"},
})

enum entityTag{
	PLAYER,
	CAMERA,
};

NLOHMANN_JSON_SERIALIZE_ENUM( entityTag, {
	{PLAYER,"PLAYER"},
	{CAMERA,"CAMERA"},
})


struct jsonComponent
{
	
};

struct jsonEntity
{
	std::vector<std::string> tags;
	std::vector<jsonComponent>  components;
};

namespace HI2{
	void to_json(nlohmann::json& j, const HI2::Color& c);
	void from_json(const nlohmann::json& j, HI2::Color& c);
}

void to_json(nlohmann::json& j, const entt::registry& registry);
void from_json(const nlohmann::json& j, entt::registry& registry);