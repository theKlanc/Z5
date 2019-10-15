#pragma once
#include "states/state_base.hpp"
#include <string>

namespace State {
	class MainMenu : public State_Base {
	  public:
		MainMenu();
		MainMenu(gameCore &gc);
		void input(double dt) override;
		void update(double dt) override;
		void draw(double dt) override;

	  private:
	};
} // namespace State
