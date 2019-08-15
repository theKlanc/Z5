#pragma once
#include "states/state_base.hpp"
#include <string>

namespace State {
	class MainMenu : public State_Base {
	  public:
		MainMenu();
		MainMenu(gameCore &gc);
		void input(float dt) override;
		void update(float dt) override;
		void draw() override;

	  private:
	};
} // namespace State
