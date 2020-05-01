#include "interactables/nodeController.hpp"
#include "services.hpp"
#include "components/brain.hpp"
#include "jsonTools.hpp"
#include "universeNode.hpp"
#include "icecream.hpp"
#include "components/velocity.hpp"

void nodeController::interact(entt::entity e)
{
	Services::enttRegistry->get<std::unique_ptr<brain>>(e)->setControlling(this);
	Services::enttRegistry->get<velocity>(e).spd = {};

}

nlohmann::json nodeController::getJson() const
{
	return nlohmann::json{{"type",NODE_CONTROLLER},{"interactable",{
	{"position",_position},{"thrustTarget",_thrustTarget}
	}}};
}

void nodeController::update(double dt, const std::bitset<HI2::BUTTON_SIZE> &down, const std::bitset<HI2::BUTTON_SIZE> &up, const std::bitset<HI2::BUTTON_SIZE> &held)
{
	if(down[HI2::BUTTON::KEY_BACKSPACE] && _exitCallback)
		_exitCallback();
	else{
		if(held[HI2::BUTTON::KEY_W]){
			_thrustTarget.y-=dt*agility;
			if(_thrustTarget.y < -1)
				_thrustTarget.y=-1;
		}
		if(held[HI2::BUTTON::KEY_A]){
			_thrustTarget.x-=dt*agility;
			if(_thrustTarget.x < -1)
				_thrustTarget.x=-1;
		}
		if(held[HI2::BUTTON::KEY_S]){
			_thrustTarget.y+=dt*agility;
			if(_thrustTarget.y > 1)
				_thrustTarget.y=1;
		}
		if(held[HI2::BUTTON::KEY_D]){
			_thrustTarget.x+=dt*agility;
			if(_thrustTarget.x > 1)
				_thrustTarget.x=1;
		}
		if(held[HI2::BUTTON::KEY_R]){
			_thrustTarget.z+=dt*agility;
			if(_thrustTarget.z > 1)
				_thrustTarget.z=1;
		}
		if(held[HI2::BUTTON::KEY_F]){
			_thrustTarget.z-=dt*agility;
			if(_thrustTarget.z < -1)
				_thrustTarget.z=-1;
		}
		if(held[HI2::BUTTON::KEY_X]){
			_thrustTarget = {};
		}
	}
	_parent->getThrustSystem()->setThrustTarget(_thrustTarget);
}

void from_json(const nlohmann::json &j, nodeController &nc)
{
	nc._position = j.at("position").get<fdd>();
	nc._thrustTarget = j.at("thrustTarget").get<point3Dd>();
}
