#pragma once
#include "states/state_base.hpp"
#include <string>
#include "UI/scene.hpp"
#include "UI/gadgets/pushButton.hpp"
#include "UI/gadgets/panel.hpp"
#include "UI/gadgets/textEntry.hpp"
#include "UI/gadgets/scrollablePanel.hpp"
#include "UI/gadgets/toggleButton.hpp"
#include "UI/gadgets/textView.hpp"

namespace State {
	class MainMenu : public State_Base {
	  public:
		MainMenu(gameCore &gc);
		void input(double dt) override;
		void update(double dt) override;
		void draw(double dt) override;

		void editPrefabCallback(std::filesystem::path s);
		void newPrefabCallback();

		void toggleEditorPanel();

	  private:
		void regenerateSavesVector();
		void regeneratePrefabsVector();

		void createMainPanel();
		void createNewGamePanel();
		void createContinuePanel();
		void createPrefabPanel();

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

		struct save {
			std::filesystem::path path;
			std::shared_ptr<panel> p;
			std::shared_ptr<pushButton> startButton;
			std::shared_ptr<pushButton> deleteButton;
		};
		struct{
			std::shared_ptr<scrollablePanel> p;
			std::vector<save> saves;
		}_continuePanel;

		struct pfb {
			std::shared_ptr<textView> txt;
			std::shared_ptr<pushButton> startButton;
		};
		struct{
			struct{
				std::shared_ptr<panel> p;
				std::shared_ptr<textEntry> pfbName;
				std::shared_ptr<textEntry> xSize;
				std::shared_ptr<textEntry> ySize;
				std::shared_ptr<textEntry> zSize;
				std::shared_ptr<pushButton> start;
			}newPrefab;
			std::shared_ptr<scrollablePanel> p;
			std::vector<pfb> prefabs;
		}_prefabPanel;

		HI2::Font _standardFont;
	};
} // namespace State
