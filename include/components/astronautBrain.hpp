#pragma once
#include "services.hpp"
#include "components/brain.hpp"
#include "entt/entt.hpp"
#include "audioManager.hpp"

class fsm_state{
public:
	virtual ~fsm_state() = 0;
	virtual fsm_state* update(double dt, entt::entity e);
	virtual fsm_state* update(double dt, entt::entity e, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held);
	virtual std::string getThoughts() const = 0;
};

class astronautBrain : public brain {
public:
	astronautBrain();
	astronautBrain(entt::entity entity);
	astronautBrain(const nlohmann::json& j,entt::entity entity);

	~astronautBrain() override;
	void _updateInternal(double dt) override;
	void update(double dt, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held) override;
	nlohmann::json getJson() const override;
	std::string getThoughts() const override;

private:
	entt::entity _entity;
	fsm_state* _currentState;
	std::vector<std::shared_ptr<fsm_state>> _states;









	class groundedState : public fsm_state{
	public:
		fsm_state* update(double dt, entt::entity e, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held) override;
		std::string getThoughts() const override;
		~groundedState() override{}
	private:
		fsm_state* _jumpingState;
		fsm_state* _airborneState;
		friend class astronautBrain;
	};

	class jumpingState : public fsm_state{
	public:
		jumpingState();
		fsm_state* update(double dt, entt::entity e, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held) override;
		std::string getThoughts() const override;
		~jumpingState() override{}
	private:
		fsm_state* _airborneState;
		fsm_state* _groundedState;
		HI2::Audio* jumpingSound;

		friend class astronautBrain;
	};

	class airborneState : public fsm_state{
	public:
		fsm_state* update(double dt, entt::entity e, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held) override;
		std::string getThoughts() const override;
		~airborneState() override{}
	private:
		fsm_state* _groundedState;
		friend class astronautBrain;
	};


};