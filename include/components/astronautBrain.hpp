#pragma once
#include "components/brain.hpp"
#include "entt/entt.hpp"
class fsm_state{
public:
	virtual ~fsm_state() = 0;
	virtual fsm_state* update(double dt, entt::entity e) = 0;
};

class astronautBrain : public brain {
public:
	astronautBrain();
	astronautBrain(entt::entity entity);
	astronautBrain(const nlohmann::json& j,entt::entity entity);

	~astronautBrain() override;
	void update(double dt) override;
	nlohmann::json getJson() const override;
private:
	class groundedState : public fsm_state{
	public:
		fsm_state* update(double dt, entt::entity e) override;
		~groundedState() override{}
	private:
		fsm_state* _jumpingState;
		fsm_state* _airborneState;
		friend class astronautBrain;
	};

	class jumpingState : public fsm_state{
	public:
		fsm_state* update(double dt, entt::entity e) override;
		~jumpingState() override{}
	private:
		fsm_state* _airborneState;
		fsm_state* _groundedState;
		friend class astronautBrain;
	};

	class airborneState : public fsm_state{
	public:
		fsm_state* update(double dt, entt::entity e) override;
		~airborneState() override{}
	private:
		fsm_state* _groundedState;
		friend class astronautBrain;
	};

	entt::entity _entity;
	fsm_state* _currentState;
	std::vector<std::shared_ptr<fsm_state>> _states;
};