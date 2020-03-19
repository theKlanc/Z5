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

void to_json(nlohmann::json& j, const entt::registry& registry)
{
	registry.each([&](auto entity) {
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
		if (registry.has<brain*>(entity))
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
		for (const nlohmann::json& j_tag : j_entity.at("tags"))
		{
			switch (j_tag.get<entityTag>())
			{
			case PLAYER:
			{
				registry.assign<entt::tag<"PLAYER"_hs>>(e);
				break;
			}
			case CAMERA:
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
			case NAME:
			{
				auto& comp = registry.assign<name>(e);
				j_component.at("content").get_to(comp);
				break;
			}
			case POSITION:
			{
				auto& comp = registry.assign<position>(e);
				j_component.at("content").get_to(comp);
				break;
			}
			case VELOCITY:
			{
				auto& comp = registry.assign<velocity>(e);
				j_component.at("content").get_to(comp);
				break;
			}
			case BODY:
			{
				auto& comp = registry.assign<body>(e);
				j_component.at("content").get_to(comp);
				break;
			}
			case DRAWABLE:
			{
				auto& comp = registry.assign<drawable>(e);
				j_component.at("content").get_to(comp);
				break;
			}
			case BRAIN:
			{
				auto& comp = registry.assign<std::unique_ptr<brain>>(e);
				std::string type = j_component.at("type").get<std::string>();
				if(type == "astronaut"){
					comp = std::make_unique<astronautBrain>(j.at("brain"),e);
				}
				break;
			}
			default:
				throw "Unknown component type";
			}

		}

	}
}
