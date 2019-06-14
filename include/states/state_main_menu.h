#pragma once
#include "states/state_base.h"
#include <string>

namespace State {
	class MainMenu : public State_Base {
	  public:
		MainMenu();
		void input() override;
		void update(float dt) override;
		void draw() override;

	  private:
	};
} // namespace State
