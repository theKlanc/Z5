#pragma once
#include "components/item.hpp"

struct resourceHarvester : public item{
	void use(entt::entity user, entt::entity i) override;
	bool operator==(const item& right) const override;

	nlohmann::json getJson() const override;
};

void from_json(const nlohmann::json &j, resourceHarvester &n);