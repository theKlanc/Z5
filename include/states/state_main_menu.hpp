#pragma once
#include "states/state_base.hpp"
#include <string>
#include "UI/scene.hpp"
#include "UI/gadgets/pushButton.hpp"
#include "UI/gadgets/panel.hpp"
#include "UI/gadgets/textEntry.hpp"
#include "UI/gadgets/toggleButton.hpp"

namespace State {
	class MainMenu : public State_Base {
	  public:
		MainMenu(gameCore &gc);
		void input(double dt) override;
		void update(double dt) override;
		void draw(double dt) override;

	  private:
		void createMainPanel();
		void createNewGamePanel();
		void createContinuePanel();

		scene _uiScene;

		struct {
			std::shared_ptr<panel> p;
			std::shared_ptr<pushButton> continueButton;
			std::shared_ptr<pushButton> newGameButton;
			std::shared_ptr<pushButton> mapEditorButton;
			std::shared_ptr<pushButton> optionsButton;
		}_mainPanel;

		struct{
			std::shared_ptr<panel> p;
			std::shared_ptr<textEntry> saveName;
			std::shared_ptr<textEntry> seed;
			std::shared_ptr<toggleButton> debug;
			std::shared_ptr<pushButton> start;
		}_newGamePanel;

		struct{
			std::shared_ptr<panel> p;
		}_continuePanel;

		HI2::Font _standardFont;
	};
} // namespace State
