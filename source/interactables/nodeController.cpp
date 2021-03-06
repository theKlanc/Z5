#include "interactables/nodeController.hpp"
#include "services.hpp"
#include "components/brain.hpp"
#include "jsonTools.hpp"
#include "universeNode.hpp"
#include "icecream.hpp"
#include "components/velocity.hpp"

nodeController::nodeController()
{
	_thrustViewer = std::make_shared<fddDisplay>(fddDisplay({},240,{},{-1,-1,-1,-1},{1,1,1,1},5,5,10));
	_thrustViewer->setPosition({HI2::getScreenWidth()-_thrustViewer->getSize().x,0});
	_scene.addGadget(_thrustViewer);

	_spdViewer = std::make_shared<fddDisplay>(fddDisplay({0,HI2::getScreenHeight()-240},240,{},{-1,-1,-1,-1},{1,1,1,1},5,5,10));
	_spdViewer->setPosition({HI2::getScreenWidth()-_spdViewer->getSize().x,HI2::getScreenHeight()-240});
	_scene.addGadget(_spdViewer);
}

nodeController::~nodeController()
{

}

void nodeController::interact(entt::entity e)
{
	Services::enttRegistry->get<std::unique_ptr<brain>>(e)->setControlling(this);
	Services::enttRegistry->get<velocity>(e).spd = {};

}

nlohmann::json nodeController::getJson() const
{
	return nlohmann::json{{"type",interactableType::NODE_CONTROLLER},{"interactable",{
	{"positions",_positions},{"thrustTarget",_thrustTarget}
			}}};
}

void nodeController::fix(point3Di dist)
{
	for(fdd& pos : _positions){
		pos+=fdd(dist);
	}
}

void nodeController::update(double dt, const std::bitset<HI2::BUTTON_SIZE> &down, const std::bitset<HI2::BUTTON_SIZE> &up, const std::bitset<HI2::BUTTON_SIZE> &held)
{
	if((down[HI2::BUTTON::KEY_BACKSPACE] || down[HI2::BUTTON::BUTTON_B]) && _exitCallback)
		_exitCallback();
	else{
		if(held[HI2::BUTTON::KEY_W] || held[HI2::BUTTON::UP]){
			_thrustTarget.y-=dt*agility;
			if(_thrustTarget.y < -1)
				_thrustTarget.y=-1;
		}
		if(held[HI2::BUTTON::KEY_A] || held[HI2::BUTTON::LEFT]){
			_thrustTarget.x-=dt*agility;
			if(_thrustTarget.x < -1)
				_thrustTarget.x=-1;
		}
		if(held[HI2::BUTTON::KEY_S] || held[HI2::BUTTON::DOWN]){
			_thrustTarget.y+=dt*agility;
			if(_thrustTarget.y > 1)
				_thrustTarget.y=1;
		}
		if(held[HI2::BUTTON::KEY_D] || held[HI2::BUTTON::RIGHT]){
			_thrustTarget.x+=dt*agility;
			if(_thrustTarget.x > 1)
				_thrustTarget.x=1;
		}
		if(held[HI2::BUTTON::KEY_R] || held[HI2::BUTTON::BUTTON_L]){
			_thrustTarget.z+=dt*agility;
			if(_thrustTarget.z > 1)
				_thrustTarget.z=1;
		}
		if(held[HI2::BUTTON::KEY_F] || held[HI2::BUTTON::BUTTON_ZL]){
			_thrustTarget.z-=dt*agility;
			if(_thrustTarget.z < -1)
				_thrustTarget.z=-1;
		}
		if(held[HI2::BUTTON::KEY_X] || held[HI2::BUTTON::BUTTON_X]){
			_thrustTarget = {};
		}
	}
	_parent->getThrustSystem()->setThrustTarget(_thrustTarget);
}

void nodeController::drawUI()
{
	_thrustViewer->setFdd({_thrustTarget.x,_thrustTarget.y,_thrustTarget.z,0});
	_spdViewer->setFdd(_parent->getVelocity());
	_scene.draw();
}

void from_json(const nlohmann::json &j, nodeController &nc)
{
	nc._positions = j.at("positions").get<std::vector<fdd>>();
	nc._thrustTarget = j.at("thrustTarget").get<point3Dd>();
}
