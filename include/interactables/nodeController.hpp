#pragma once
#include "interactable.hpp"
#include "controllable.hpp"

class nodeController : public interactable, public controllable{
	void interact(entt::entity e) override;
	json getJson() const override;
	void update(double dt, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held) override;
private:

	point3Dd _thrustTarget = {};
	double agility = 0.25;

	friend void from_json(const json& j, nodeController& nc);

};

void from_json(const json& j, nodeController& nc);