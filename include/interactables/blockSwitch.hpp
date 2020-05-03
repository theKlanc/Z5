#pragma once
#include "interactable.hpp"
#include "block.hpp"

class blockSwitch : public interactable{
public:
	blockSwitch();
	void interact(entt::entity e) override;
	json getJson() const override;
private:
	std::vector<std::pair<metaBlock,point3Di>> _blocks;
	friend void from_json(const json& j, blockSwitch& bs);
};

void from_json(const json& j, blockSwitch& bs);