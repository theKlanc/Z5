#include "interactables/nodeController.hpp"
#include "services.hpp"
#include "components/brain.hpp"
#include "jsonTools.hpp"
#include "universeNode.hpp"

void nodeController::interact(entt::entity e)
{
	Services::enttRegistry->get<std::unique_ptr<brain>>(e)->setControlling(this);
}

nlohmann::json nodeController::getJson() const
{
	return nlohmann::json{{"type",NODE_CONTROLLER},{"interactable",{}}};
}

void nodeController::update(double dt, const std::bitset<HI2::BUTTON_SIZE> &down, const std::bitset<HI2::BUTTON_SIZE> &up, const std::bitset<HI2::BUTTON_SIZE> &held)
{
	if(down[HI2::BUTTON::KEY_ESCAPE])
		_exitCallback();
	else{
		if(down[HI2::BUTTON::KEY_W]){
			_parent->applyThrusters(dt);
		}
	}
}
