#pragma once
#include "entt/entity/registry.hpp"
#include "json.hpp"
#include "components/inventory.hpp"

struct hand{
	std::optional<entt::entity> _item;

	void nextItem(inventory& inv);
	void previousItem(inventory& inv);
	unsigned index;
	unsigned toolbarSize = 4;

	void select(inventory& inv);
};

void to_json(nlohmann::json& j, const hand& n);
void from_json(const nlohmann::json& j, hand& n);