#pragma once
#include <vector>
#include <string>
#include "json.hpp"
#include "entt/entity/fwd.hpp"
#include "HI2.hpp"
#include "fdd.hpp"
#include "interactable.hpp"

enum class componentType{
	NAME,
	POSITION,
	VELOCITY,
	BODY,
	DRAWABLE,
	BRAIN,
	PROJECTILE,
	HEALTH,
};

NLOHMANN_JSON_SERIALIZE_ENUM( componentType, {
	{componentType::NAME,"NAME"},
	{componentType::POSITION,"POSITION"},
	{componentType::VELOCITY,"VELOCITY"},
	{componentType::BODY,"BODY"},
	{componentType::DRAWABLE,"DRAWABLE"},
	{componentType::BRAIN,"BRAIN"},
	{componentType::PROJECTILE,"PROJECTILE"},
	{componentType::HEALTH,"HEALTH"},
})

enum class interactableType{
	BLOCK_SWITCH,
	NODE_CONTROLLER
};

NLOHMANN_JSON_SERIALIZE_ENUM( interactableType, {
	{interactableType::BLOCK_SWITCH,"BLOCK_SWITCH"},
	{interactableType::NODE_CONTROLLER,"NODE_CONTROLLER"},
})

enum class entityTag{
	PLAYER,
	CAMERA,
	ACTIVE,
};

NLOHMANN_JSON_SERIALIZE_ENUM( entityTag, {
	{entityTag::PLAYER,"PLAYER"},
	{entityTag::CAMERA,"CAMERA"},
	{entityTag::ACTIVE,"ACTIVE"},
})

namespace HI2{
	void to_json(nlohmann::json& j, const HI2::Color& c);
	void from_json(const nlohmann::json& j, HI2::Color& c);
}

void to_json(nlohmann::json& j, const entt::registry& registry);
void from_json(const nlohmann::json& j, entt::registry& registry);


void to_json(nlohmann::json& j, const point3Dd& p);
void from_json(const nlohmann::json& j, point3Dd& p);

void to_json(nlohmann::json& j, const point3Di& p);
void from_json(const nlohmann::json& j, point3Di& p);


std::unique_ptr<interactable> getInteractableFromJson(const nlohmann::json& j);