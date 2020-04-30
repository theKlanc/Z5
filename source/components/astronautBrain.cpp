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
	_currentState = _currentState->update(dt,_entity);
}

void astronautBrain::update(double dt, const std::bitset<HI2::BUTTON_SIZE> &down, const std::bitset<HI2::BUTTON_SIZE> &up, const std::bitset<HI2::BUTTON_SIZE> &held)
{

	_currentState = _currentState->update(dt,_entity,down,up,held);
}

nlohmann::json astronautBrain::getJson() const {
	return json{{"type", "astronaut"}, {"brain", {}}};
}
std::string astronautBrain::getThoughts() const
{
	return _currentState->getThoughts();
}
fsm_state *astronautBrain::groundedState::update(double dt, entt::entity e, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held) {
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

	return this;
}

std::string astronautBrain::groundedState::getThoughts() const
{
	return "grounded";
}

astronautBrain::jumpingState::jumpingState() : fsm_state()
{
	jumpingSound = Services::audio.loadAudio("sfx/boing");
}

fsm_state *astronautBrain::jumpingState::update(double dt, entt::entity e, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held) {
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

fsm_state *astronautBrain::airborneState::update(double dt, entt::entity e, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held)
{
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
	if(held[HI2::BUTTON::KEY_F]){
		vel.spd.z-=13*dt;
	}
	if(held[HI2::BUTTON::KEY_R]){
		vel.spd.z+=13*dt;
	}
	return this;
}

std::string astronautBrain::airborneState::getThoughts() const
{
	return "woohoooooooooooooo";
}

fsm_state::~fsm_state(){}
fsm_state *fsm_state::update(double dt, entt::entity e)
{
	return this;
}

fsm_state *fsm_state::update(double dt, entt::entity e, const std::bitset<HI2::BUTTON_SIZE> &down, const std::bitset<HI2::BUTTON_SIZE> &up, const std::bitset<HI2::BUTTON_SIZE> &held)
{
	return this;
}
