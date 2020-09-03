#pragma once
#include "components/item.hpp"
#include "block.hpp"

struct placeableBlock : public item{
	placeableBlock(const nlohmann::json& j);
	placeableBlock(baseBlock& b) : _block(b){}
	baseBlock& _block;
	void use(entt::entity user, entt::entity i) override;
	bool operator==(const item& right) const override;
	nlohmann::json getJson() const override;
};

void from_json(const nlohmann::json& j, placeableBlock& n);
