#include "states/state_main_menu.hpp"
#include "states/state_playing.hpp"
#include "UI/scene.hpp"
#include "UI/gadgets/imageView.hpp"
#include "UI/gadgets/basicPanel.hpp"
#include "UI/gadgets/textView.hpp"
#include "UI/gadgets/textEntry.hpp"
#include "UI/gadgets/toggleButton.hpp"
#include "UI/gadgets/basicScrollablePanel.hpp"
#include "UI/gadgets/basicTextEntry.hpp"
#include "gameCore.hpp"
#include "states/state_demo.hpp"


State::MainMenu::MainMenu(gameCore &gc):State_Base(gc),_standardFont("data/fonts/test.ttf")
{

	HI2::Texture& bg = *_core->getGraphics().loadTexture("bg");

	
	_uiScene.addGadget(std::make_shared<imageView>(point2D{0,0},point2D{1280,720},bg));
	_uiScene.addGadget(std::make_shared<textView>(point2D{HI2::getScreenWidth()/2-110,50},point2D{220,50},"Z5",_standardFont,200,HI2::Color::White));
	
	createMainPanel();

	createContinuePanel();

	createNewGamePanel();

	HI2::setBackgroundColor(HI2::Color(0, 0, 0, 255));
}

void State::MainMenu::input(double dt) {
	if(HI2::getKeysDown() & HI2::BUTTON::KEY_ESCAPE){
		if(_mainPanel.p->isActive())
		{
			_core->popState();
		}
		else if(_continuePanel.p->isActive()){
			_continuePanel.p->setActive(false);
			_continuePanel.p->setVisible(false);
			_mainPanel.p->setActive(true);
			_mainPanel.p->setVisible(true);

			_uiScene.select(_mainPanel.p);
		}
		else if(_newGamePanel.p->isActive()){
			_newGamePanel.p->setActive(false);
			_newGamePanel.p->setVisible(false);
			_mainPanel.p->setActive(true);
			_mainPanel.p->setVisible(true);

			_uiScene.select(_mainPanel.p);
		}
	}
}

void State::MainMenu::update(double dt) {
	_uiScene.update(HI2::getKeysDown(),HI2::getKeysUp(),HI2::getKeysHeld(),HI2::getTouchPos(),dt);
	if(_mainPanel.continueButton->isRisingInside())
	{
		_mainPanel.continueButton->update(dt);
		_continuePanel.p->setActive(true);
		_continuePanel.p->setVisible(true);
		_mainPanel.p->setActive(false);
		_mainPanel.p->setVisible(false);

		_uiScene.select(_continuePanel.p);
	}
	if(_mainPanel.newGameButton->isRisingInside())
	{
		_mainPanel.newGameButton->update(dt);
		_newGamePanel.p->setActive(true);
		_newGamePanel.p->setVisible(true);
		_mainPanel.p->setActive(false);
		_mainPanel.p->setVisible(false);

		_uiScene.select(_newGamePanel.p);
	}
	if(_newGamePanel.start->isRisingInside())
	{
		if(!_newGamePanel.seed->isEmpty() && !_newGamePanel.saveName->isEmpty()){
			_newGamePanel.seed->setHintColor(HI2::Color::Grey);
			_newGamePanel.saveName->setHintColor(HI2::Color::Grey);

			_newGamePanel.start->update(dt);
			_newGamePanel.p->setActive(false);
			_newGamePanel.p->setVisible(false);
			_mainPanel.p->setActive(true);
			_mainPanel.p->setVisible(true);

			_uiScene.select(_mainPanel.p);

			_core->pushState(std::make_unique<State::Playing>(*_core,_newGamePanel.saveName->getText(),std::stoi(_newGamePanel.seed->getText(),nullptr)));

			_newGamePanel.saveName->setText("");
			_newGamePanel.seed->setText(std::to_string(rand()%100000));
		}
		else{
			if(_newGamePanel.seed->isEmpty())
				_newGamePanel.seed->setHintColor(HI2::Color::Red);
			if(_newGamePanel.saveName->isEmpty())
				_newGamePanel.saveName->setHintColor(HI2::Color::Red);
		}
	}
}

void State::MainMenu::draw(double dt) {
	HI2::startFrame();
	_uiScene.draw();
	HI2::endFrame();
}

void State::MainMenu::createMainPanel()
{
	_mainPanel.p = std::make_shared<basicPanel>(point2D{HI2::getScreenWidth()/2-300/2,300},point2D{300,270},HI2::Color::Grey);

	std::shared_ptr<gadget> last;

	_mainPanel.continueButton = std::make_shared<pushButton>(point2D{30,30},point2D{30,30},"");
	_mainPanel.p->addGadget(_mainPanel.continueButton);
	last = _mainPanel.continueButton;
	_mainPanel.p->addGadget(std::make_shared<textView>(point2D{80,30},point2D{100,30},"Continue",_standardFont,30,HI2::Color::Black));

	_mainPanel.newGameButton = std::make_shared<pushButton>(point2D{30,90},point2D{30,30},"");
	_mainPanel.p->addGadget(_mainPanel.newGameButton);
	last->setDown(_mainPanel.newGameButton.get());
	_mainPanel.newGameButton->setUp(last.get());
	last=_mainPanel.newGameButton;
	_mainPanel.p->addGadget(std::make_shared<textView>(point2D{80,90},point2D{100,30},"New Game",_standardFont,30,HI2::Color::Black));

	_mainPanel.mapEditorButton = std::make_shared<pushButton>(point2D{30,150},point2D{30,30},"");
	_mainPanel.p->addGadget(_mainPanel.mapEditorButton);
	last->setDown(_mainPanel.mapEditorButton.get());
	_mainPanel.mapEditorButton->setUp(last.get());
	last=_mainPanel.mapEditorButton;
	_mainPanel.p->addGadget(std::make_shared<textView>(point2D{80,150},point2D{100,30},"Map editor",_standardFont,30,HI2::Color::Black));

	_mainPanel.optionsButton = std::make_shared<pushButton>(point2D{30,210},point2D{30,30},"");
	_mainPanel.p->addGadget(_mainPanel.optionsButton);
	last->setDown(_mainPanel.optionsButton.get());
	_mainPanel.optionsButton->setUp(last.get());
	last=_mainPanel.optionsButton;
	_mainPanel.p->addGadget(std::make_shared<textView>(point2D{80,210},point2D{100,30},"Options",_standardFont,30,HI2::Color::Black));

	_uiScene.addGadget(_mainPanel.p);
}

void State::MainMenu::createNewGamePanel()
{
	_newGamePanel.p = std::make_shared<basicPanel>(point2D{HI2::getScreenWidth()/2-500/2,250},point2D{500,270},HI2::Color::Grey);
	_newGamePanel.p->setActive(false);
	_newGamePanel.p->setVisible(false);
	std::shared_ptr<gadget> last;
	_uiScene.addGadget(_newGamePanel.p);

	_newGamePanel.saveName = std::make_shared<basicTextEntry>(point2D{30,30},point2D{_newGamePanel.p->getSize().x-60,30},_standardFont,30,"","Save Name",HI2::Color::White,HI2::Color::Black,HI2::Color::Grey);
	_newGamePanel.p->addGadget(_newGamePanel.saveName);
	last = _newGamePanel.saveName;

	_newGamePanel.seed = std::make_shared<basicTextEntry>(point2D{30,90},point2D{120,30},_standardFont,30,std::to_string(rand()%100000),"",HI2::Color::White,HI2::Color::Black,HI2::Color::Grey);
	_newGamePanel.p->addGadget(_newGamePanel.seed);
	last->setDown(_newGamePanel.seed.get());
	_newGamePanel.seed->setUp(last.get());
	last = _newGamePanel.seed;
	_newGamePanel.p->addGadget(std::make_shared<textView>(point2D{165,90},point2D{120,30},"Seed",_standardFont,30,HI2::Color::Black));

	_newGamePanel.debug = std::make_shared<toggleButton>(point2D{30,150},point2D{30,30});
	_newGamePanel.p->addGadget(_newGamePanel.debug);
	last->setDown(_newGamePanel.debug.get());
	_newGamePanel.debug->setUp(last.get());
	last = _newGamePanel.debug;
	_newGamePanel.p->addGadget(std::make_shared<textView>(point2D{75,150},point2D{120,30},"DEBUG",_standardFont,30,HI2::Color::Black));

	_newGamePanel.start = std::make_shared<pushButton>(point2D{_newGamePanel.p->getSize().x/2-15,210},point2D{30,30});
	_newGamePanel.p->addGadget(_newGamePanel.start);
	last->setDown(_newGamePanel.start.get());
	_newGamePanel.start->setUp(last.get());
	last = _newGamePanel.start;
}

void State::MainMenu::createContinuePanel()
{
	_continuePanel.p = std::make_shared<basicScrollablePanel>(point2D{HI2::getScreenWidth()/2-300/2,300},point2D{300,270},point2D{300,270},HI2::Color::Grey);
	_continuePanel.p->setActive(false);
	_continuePanel.p->setVisible(false);
	std::shared_ptr<gadget> last;

	//_continueButton = std::make_shared<pushButton>(point2D{30,30},point2D{30,30},"");
	//mainPanel->addGadget(_continueButton);
	//last = _continueButton;
	//mainPanel->addGadget(std::make_shared<textView>(point2D{80,30},point2D{100,30},"Continue",_standardFont,30,HI2::Color::Black));
	//
	//_newGameButton = std::make_shared<pushButton>(point2D{30,90},point2D{30,30},"");
	//mainPanel->addGadget(_newGameButton);
	//
	//last=_newGameButton;
	//mainPanel->addGadget(std::make_shared<textView>(point2D{80,90},point2D{100,30},"New Game",_standardFont,30,HI2::Color::Black));

	_uiScene.addGadget(_continuePanel.p);
}
