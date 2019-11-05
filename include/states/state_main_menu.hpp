#pragma once
#include "states/state_base.hpp"
#include <string>
#include "UI/scene.hpp"
#include "UI/gadgets/pushButton.hpp"

namespace State {
	class MainMenu : public State_Base {
	  public:
		MainMenu(gameCore &gc);
		void input(double dt) override;
		void update(double dt) override;
		void draw(double dt) override;

	  private:
		scene _uiScene;
		std::shared_ptr<pushButton> _continueButton;
		std::shared_ptr<pushButton> _newGameButton;
		std::shared_ptr<pushButton> _demoButton;
		HI2::Font _standardFont;
	};
} // namespace State
