#pragma once
#include "entt/entity/registry.hpp"
#include "json.hpp"

struct item{
	int amount = 1;
	virtual void use(entt::entity user, entt::entity i) = 0;
	virtual bool operator==(const item& right) const = 0;
	virtual nlohmann::json getJson() const = 0;

	static std::unordered_map<int,entt::entity> _itemIDLUT;
};

