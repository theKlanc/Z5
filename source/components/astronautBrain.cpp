#include "components/astronautBrain.hpp"
#include "components/position.hpp"
#include "components/velocity.hpp"
#include <iostream>

astronautBrain::astronautBrain(){}

astronautBrain::astronautBrain(entt::entity entity) : brain(){
	_entity = entity;
	auto gs = std::make_shared<groundedState>();
	auto js = std::make_shared<jumpingState>();
	auto as = std::make_shared<airborneState>();
	gs->_jumpingState = js.get();
	gs->_airborneState = as.get();
	js->_airborneState = as.get();
	as->_groundedState = gs.get();
	js->_groundedState = gs.get();
	_currentState = gs.get();
	_states.push_back(std::move(gs));
	_states.push_back(std::move(js));
	_states.push_back(std::move(as));
}

astronautBrain::astronautBrain(const nlohmann::json &j, entt::entity entity) : astronautBrain(entity){

}

astronautBrain::~astronautBrain(){}

void astronautBrain::update(double dt) {
	if (auto result = _currentState->update(dt,_entity); result){
		_currentState = result;
	}
}

nlohmann::json astronautBrain::getJson() const {
	return json{{"type", "astronaut"}, {"brain", {}}};
}
std::string astronautBrain::getThoughts() const
{
	return _currentState->getThoughts();
}
fsm_state *astronautBrain::groundedState::update(double dt, entt::entity e) {
	auto& down = HI2::getKeysDown();
	auto& held = HI2::getKeysHeld();

	auto pos = Services::enttRegistry->get<position>(e);
	pos.pos.z-=0.3;
	if(!pos.parent->getBlock(pos.pos.getPoint3Di()).base->solid)
	{
		return _airborneState;
	}
	if(down[HI2::BUTTON::KEY_SPACE]) {
		return _jumpingState;
	}
	auto& vel = Services::enttRegistry->get<velocity>(e);
	if(held[HI2::BUTTON::KEY_A] && vel.spd.x > -5.0f){
		vel.spd.x-=5.0f * dt;
	}
	if(held[HI2::BUTTON::KEY_D] && vel.spd.x < 5.0f){
		vel.spd.x+=5.0f * dt;
	}
	if(held[HI2::BUTTON::KEY_W] && vel.spd.y > -5.0f){
		vel.spd.y-=5.0f * dt;
	}
	if(held[HI2::BUTTON::KEY_S] && vel.spd.y < 5.0f){
		vel.spd.y+=5.0f * dt;
	}

	return nullptr;
}

std::string astronautBrain::groundedState::getThoughts() const
{
	return "grounded";
}

astronautBrain::jumpingState::jumpingState()
{
	jumpingSound = Services::audio.loadAudio("sfx/boing");
}

fsm_state *astronautBrain::jumpingState::update(double dt, entt::entity e) {
	//auto pos = Services::enttRegistry->get<position>(e);
	//pos.pos.z-=0.3;
	//if(!pos.parent->getBlock(pos.pos.getPoint3Di()).base->solid)
	//{
	//	return _airborneState;
	//}
	auto& vel = Services::enttRegistry->get<velocity>(e);
	vel.spd.z+=10;
	HI2::playSound(*jumpingSound);
	return _airborneState;
}

std::string astronautBrain::jumpingState::getThoughts() const
{
	return "jumping";
}

fsm_state *astronautBrain::airborneState::update(double dt, entt::entity e)
{
	auto& held = HI2::getKeysHeld();
	auto pos = Services::enttRegistry->get<position>(e);
	pos.pos.z-=0.1;
	if(pos.parent->getBlock(pos.pos.getPoint3Di()).base->solid)
	{
		return _groundedState;
	}
	auto& vel = Services::enttRegistry->get<velocity>(e);
	if(held[HI2::BUTTON::KEY_A]){
		vel.spd.x-=2*dt;
	}
	if(held[HI2::BUTTON::KEY_D]){
		vel.spd.x+=2*dt;
	}
	if(held[HI2::BUTTON::KEY_W]){
		vel.spd.y-=2*dt;
	}
	if(held[HI2::BUTTON::KEY_S]){
		vel.spd.y+=2*dt;
	}
	return nullptr;
}

std::string astronautBrain::airborneState::getThoughts() const
{
	return "woohoooooooooooooo";
}

fsm_state::~fsm_state(){}