#pragma once
#include "interactable.hpp"
#include "controllable.hpp"
#include "UI/scene.hpp"
#include "UI/customGadgets/fddViewer.hpp"
#include "HI2.hpp"

class nodeController : public interactable, public controllable{
public:
	nodeController();
	~nodeController() override;
	void interact(entt::entity e) override;
	json getJson() const override;
	void fix(point3Di dist) override;
	void update(double dt, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held) override;
private:

	point3Dd _thrustTarget = {};
	double agility = 0.5;

	std::shared_ptr<fddViewer> _thrustViewer;
	std::shared_ptr<fddViewer> _spdViewer;
	scene _scene;
	friend void from_json(const json& j, nodeController& nc);

	// controllable interface
public:
	void drawUI() override;
};

void from_json(const json& j, nodeController& nc);