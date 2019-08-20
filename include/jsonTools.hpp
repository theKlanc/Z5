#pragma once
#include <vector>
#include <string>
#include "json.hpp"
#include "entt/entity/fwd.hpp"

enum componentType{
	NAME,
	POSITION,
	VELOCITY,
	BODY,
	DRAWABLE,
};

NLOHMANN_JSON_SERIALIZE_ENUM( componentType, {
	{NAME,"NAME"},
	{POSITION,"POSITION"},
	{VELOCITY,"VELOCITY"},
	{BODY,"BODY"},
	{DRAWABLE,"DRAWABLE"},
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


void to_json(nlohmann::json& j, const entt::registry& registry);
void from_json(const nlohmann::json& j, entt::registry& registry);