#include "jsonTools.hpp"
#include "entt/entity/registry.hpp"
#include "entt/entity/helper.hpp"
#include "components/name.hpp"
#include "components/position.hpp"
#include "components/velocity.hpp"
#include "components/drawable.hpp"
#include "components/body.hpp"
#include "components/brain.hpp"
#include "components/astronautBrain.hpp"
#include "interactables/nodeController.hpp"
#include "interactables/blockSwitch.hpp"

void to_json(nlohmann::json& j, const entt::registry& registry)
{
	registry.each([&](auto entity) {
		if (!registry.has<entt::tag<"PERMANENT"_hs>>(entity)){
			return;
		}
		nlohmann::json j_tags;
		if (registry.has<entt::tag<"PLAYER"_hs>>(entity))
		{
			j_tags.push_back("PLAYER");
		}
		if (registry.has<entt::tag<"CAMERA"_hs>>(entity))
		{
			j_tags.push_back("CAMERA");
		}
		nlohmann::json j_components;
		if (registry.has<name>(entity))
		{
			nlohmann::json j_component = nlohmann::json{ {"type","NAME"},{ "content",registry.get<name>(entity) } };
			j_components.push_back(j_component);
		}
		if (registry.has<position>(entity))
		{
			nlohmann::json j_component = nlohmann::json{ {"type","POSITION"},{ "content",registry.get<position>(entity) } };
			j_components.push_back(j_component);
		}
		if (registry.has<velocity>(entity))
		{
			nlohmann::json j_component = nlohmann::json{ {"type","VELOCITY"},{ "content",registry.get<velocity>(entity) } };
			j_components.push_back(j_component);
		}
		if (registry.has<drawable>(entity))
		{
			nlohmann::json j_component = nlohmann::json{ {"type","DRAWABLE"},{ "content",registry.get<drawable>(entity) } };
			j_components.push_back(j_component);
		}
		if (registry.has<body>(entity))
		{
			nlohmann::json j_component = nlohmann::json{ {"type","BODY"},{ "content",registry.get<body>(entity) } };
			j_components.push_back(j_component);
		}
		if (registry.has<std::unique_ptr<brain>>(entity))
		{
			nlohmann::json j_component = nlohmann::json{ {"type","BRAIN"},{ "content",*registry.get<std::unique_ptr<brain>>(entity) } };
			j_components.push_back(j_component);
		}
		j.push_back(nlohmann::json{ {"tags",j_tags},{"components",j_components} });
	});
}

void from_json(const nlohmann::json& j, entt::registry& registry)
{
	for (const nlohmann::json& j_entity : j)
	{
		entt::entity e = registry.create();
		registry.assign<entt::tag<"PERMANENT"_hs>>(e);
		for (const nlohmann::json& j_tag : j_entity.at("tags"))
		{
			switch (j_tag.get<entityTag>())
			{
			case entityTag::PLAYER:
			{
				registry.assign<entt::tag<"PLAYER"_hs>>(e);
				break;
			}
			case entityTag::CAMERA:
			{
				registry.assign<entt::tag<"CAMERA"_hs>>(e);
				break;
			}
			default:
				throw "Unknown tag type";
			}

		}
		for (const nlohmann::json& j_component : j_entity.at("components"))
		{
			switch (j_component.at("type").get<componentType>())
			{
			case componentType::NAME:
			{
				auto& comp = registry.assign<name>(e);
				j_component.at("content").get_to(comp);
				break;
			}
			case componentType::POSITION:
			{
				auto& comp = registry.assign<position>(e);
				j_component.at("content").get_to(comp);
				break;
			}
			case componentType::VELOCITY:
			{
				auto& comp = registry.assign<velocity>(e);
				j_component.at("content").get_to(comp);
				break;
			}
			case componentType::BODY:
			{
				auto& comp = registry.assign<body>(e);
				j_component.at("content").get_to(comp);
				break;
			}
			case componentType::DRAWABLE:
			{
				auto& comp = registry.assign<drawable>(e);
				j_component.at("content").get_to(comp);
				break;
			}
			case componentType::BRAIN:
			{
				auto& comp = registry.assign<std::unique_ptr<brain>>(e);
				std::string type = j_component.at("content").at("type").get<std::string>();
				if(type == "astronaut"){
					comp = std::make_unique<astronautBrain>(j_component.at("content").at("brain"),e);
				}
				break;
			}
			default:
				throw "Unknown component type";
			}
		}
	}
}

void HI2::to_json(nlohmann::json &j, const HI2::Color &color)
{
	j = json{{"r",color.r},{"g",color.g},{"b",color.b},{"a",color.a}};
}

void HI2::from_json(const nlohmann::json &j, HI2::Color &color)
{
	color.r = j.at("r").get<unsigned char>();
	color.g = j.at("g").get<unsigned char>();
	color.b = j.at("b").get<unsigned char>();
	color.a = j.at("a").get<unsigned char>();
}


void to_json(nlohmann::json &j, const point3Dd &p)
{
	j = nlohmann::json{{"x",p.x},{"y",p.y},{"z",p.z}};
}

void from_json(const nlohmann::json &j, point3Dd &p)
{
	p.x = j.at("x").get<double>();
	p.y = j.at("y").get<double>();
	p.z = j.at("z").get<double>();
}
void to_json(nlohmann::json &j, const point3Di &p)
{
	j = nlohmann::json{{"x",p.x},{"y",p.y},{"z",p.z}};
}

void from_json(const nlohmann::json &j, point3Di &p)
{
	p.x = j.at("x").get<double>();
	p.y = j.at("y").get<double>();
	p.z = j.at("z").get<double>();
}

std::unique_ptr<interactable> getInteractableFromJson(const nlohmann::json &j)
{
	interactableType type = j.at("type").get<interactableType>();
	switch(type){
	case interactableType::BLOCK_SWITCH:
	{
			blockSwitch bc;
			j.at("interactable").get_to(bc);
			return std::make_unique<blockSwitch>(bc);
	}
	case interactableType::NODE_CONTROLLER:
	{
		nodeController nc;
		j.at("interactable").get_to(nc);
		return std::make_unique<nodeController>(nc);
	}
	}
	return std::unique_ptr<interactable>();
}
