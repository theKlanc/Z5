#include "states/state_main_menu.hpp"
#include "states/state_playing.hpp"
#include "UI/scene.hpp"
#include "UI/gadgets/imageView.hpp"
#include "UI/gadgets/basicPanel.hpp"
#include "UI/gadgets/textView.hpp"
#include "gameCore.hpp"

State::MainMenu::MainMenu(gameCore &gc):State_Base(gc),_standardFont("data/fonts/test.ttf")
{

	HI2::Texture& bg = *_core->getGraphics().loadTexture("bg");
	std::shared_ptr<gadget> last;

	_uiScene.addGadget(std::make_shared<imageView>(point2D{0,0},point2D{1280,720},bg));
	//_uiScene.addGadget(std::make_shared<textView>(point2D{200,200},point2D{1080,520},"Z5",_standardFont,50,HI2::Color::White));

	std::shared_ptr<basicPanel> panel = std::make_shared<basicPanel>(point2D{500,300},point2D{500,300},HI2::Color::Grey);
	_playButton = std::make_shared<button>(point2D{30,30},point2D{30,30},"");
	//last = _playButton;
	//std::shared_ptr<button> createButton = std::make_shared<button>(point2D{30,90},point2D{30,30},"");

	//panel->addGadget(_playButton);
	//panel->addGadget(createButton);
	//createButton->setUp(_playButton.get());
	//_playButton->setDown(createButton.get());




	_uiScene.addGadget(panel);
	HI2::setBackgroundColor(HI2::Color(0, 0, 0, 255));
}

void State::MainMenu::input(double dt) {

}

void State::MainMenu::update(double dt) {
	_uiScene.update(HI2::getKeysDown(),HI2::getKeysUp(),HI2::getKeysHeld(),HI2::getTouchPos(),dt);
	if(_playButton->isRisingInside())
		_core->pushState(std::make_unique<State::Playing>(*_core,"default",0));
}

void State::MainMenu::draw(double dt) {
	HI2::startFrame();
	_uiScene.draw();
	HI2::endFrame();
}
