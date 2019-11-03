#pragma once
#include "states/state_base.hpp"
#include <string>
#include "UI/scene.hpp"
#include "UI/gadgets/button.hpp"

namespace State {
	class MainMenu : public State_Base {
	  public:
		MainMenu();
		MainMenu(gameCore &gc);
		void input(double dt) override;
		void update(double dt) override;
		void draw(double dt) override;

	  private:
		scene _uiScene;
		std::shared_ptr<button> _playButton;
	};
} // namespace State
