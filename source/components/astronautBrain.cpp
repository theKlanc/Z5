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
fsm_state *astronautBrain::groundedState::update(double dt, entt::entity e) {
	auto& down = HI2::getKeysDown();
	auto& held = HI2::getKeysDown();

	if(down[HI2::BUTTON::KEY_SPACE]) {
		return _jumpingState;
	}
	auto& vel = Services::enttRegistry->get<velocity>(e);
	if(held[HI2::BUTTON::KEY_A]){
		vel.spd.x-=2.0f * dt;
		exit(0);
	}
	if(held[HI2::BUTTON::KEY_D]){
		vel.spd.x+=2.0f * dt;
	}
	if(held[HI2::BUTTON::KEY_W]){
		vel.spd.y-=2.0f * dt;
	}
	if(held[HI2::BUTTON::KEY_S]){
		vel.spd.y+=2.0f * dt;
	}

	return nullptr;
}

fsm_state *astronautBrain::jumpingState::update(double dt, entt::entity e) { return nullptr; }

fsm_state *astronautBrain::airborneState::update(double dt, entt::entity e) { return nullptr; }

fsm_state::~fsm_state(){}