#include "states/state_main_menu.hpp"
#include "states/state_playing.hpp"
#include "UI/scene.hpp"
#include "UI/gadgets/imageView.hpp"
#include "UI/gadgets/basicPanel.hpp"
#include "UI/gadgets/textView.hpp"
#include "UI/gadgets/textEntry.hpp"
#include "UI/gadgets/toggleButton.hpp"
#include "UI/gadgets/scrollablePanel.hpp"
#include "gameCore.hpp"
#include "states/state_demo.hpp"


State::MainMenu::MainMenu(gameCore &gc):State_Base(gc),_standardFont("data/fonts/test.ttf")
{

	HI2::Texture& bg = *_core->getGraphics().loadTexture("bg");
	std::shared_ptr<gadget> last;
	
	_uiScene.addGadget(std::make_shared<imageView>(point2D{0,0},point2D{1280,720},bg));
	_uiScene.addGadget(std::make_shared<textView>(point2D{0,0},point2D{100,50},"Z5",_standardFont,200,HI2::Color::White));
	

	std::shared_ptr<scrollablePanel> panel = std::make_shared<scrollablePanel>(point2D{500,300},point2D{500,300},400);
	
	_continueButton = std::make_shared<pushButton>(point2D{30,30},point2D{30,30},"");
	panel->addGadget(_continueButton);
	last = _continueButton;
	panel->addGadget(std::make_shared<textView>(point2D{80,30},point2D{100,30},"Continue",_standardFont,30,HI2::Color::Black));
	
	_newGameButton = std::make_shared<toggleButton>(point2D{30,90},point2D{30,30},"");
	panel->addGadget(_newGameButton);
	last->setDown(_newGameButton.get());
	_newGameButton->setUp(last.get());
	last=_newGameButton;
	panel->addGadget(std::make_shared<textView>(point2D{80,90},point2D{100,30},"New Game",_standardFont,30,HI2::Color::Black));

	_demoButton = std::make_shared<pushButton>(point2D{30,150},point2D{30,30},"");
	panel->addGadget(_demoButton);
	last->setDown(_demoButton.get());
	_demoButton->setUp(last.get());
	last=_demoButton;
	panel->addGadget(std::make_shared<textView>(point2D{80,150},point2D{100,30},"Demo",_standardFont,30,HI2::Color::Black));
	
	std::shared_ptr<textEntry> txt = std::make_shared<textEntry>(point2D{30,210},point2D{90,30},_standardFont,30,"","savename");
	panel->addGadget(txt);
	last->setDown(txt.get());
	txt->setUp(last.get());
	last=txt;


	_uiScene.addGadget(panel);

	HI2::setBackgroundColor(HI2::Color(0, 0, 0, 255));
}

void State::MainMenu::input(double dt) {
	if(HI2::getKeysDown() & HI2::BUTTON::KEY_ESCAPE)
		_core->popState();
}

void State::MainMenu::update(double dt) {
	_uiScene.update(HI2::getKeysDown(),HI2::getKeysUp(),HI2::getKeysHeld(),HI2::getTouchPos(),dt);
	if(_continueButton->isRisingInside())
		_core->pushState(std::make_unique<State::Playing>(*_core,"default",0));
	if(_demoButton->isRisingInside())
		_core->pushState(std::make_unique<State::Demo>(*_core));
}

void State::MainMenu::draw(double dt) {
	HI2::startFrame();
	_uiScene.draw();
	HI2::endFrame();
}
