#include "states/state_main_menu.hpp"
#include "states/state_playing.hpp"
#include "UI/scene.hpp"
#include "gameCore.hpp"
State::MainMenu::MainMenu() {}

State::MainMenu::MainMenu(gameCore &gc):State_Base(gc)
{
	_playButton = std::make_shared<button>(point2D{300,300},point2D{30,30},"");
	_uiScene.addGadget(_playButton);
	HI2::setBackgroundColor(HI2::Color(0, 0, 0, 255));
}

void State::MainMenu::input(double dt) {

}

void State::MainMenu::update(double dt) {
	_uiScene.update(HI2::getKeysDown(),HI2::getKeysUp(),HI2::getKeysHeld(),HI2::getTouchPos(),dt);
	if(_playButton->isRising())
		_core->pushState(std::make_unique<State::Playing>(*_core,"default",0));
}

void State::MainMenu::draw(double dt) {
	HI2::startFrame();
	_uiScene.draw();
	HI2::endFrame();
}
