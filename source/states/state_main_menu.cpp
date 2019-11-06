#include "states/state_main_menu.hpp"
#include "states/state_playing.hpp"
#include "UI/scene.hpp"
#include "UI/gadgets/imageView.hpp"
#include "UI/gadgets/basicPanel.hpp"
#include "UI/gadgets/textView.hpp"
#include "UI/gadgets/textEntry.hpp"
#include "UI/gadgets/imagePushButton.hpp"
#include "UI/gadgets/toggleButton.hpp"
#include "UI/gadgets/basicScrollablePanel.hpp"
#include "UI/gadgets/basicTextEntry.hpp"
#include "gameCore.hpp"
#include "states/state_demo.hpp"
#include <iostream>
#include "UI/gadgets/imageToggleButton.hpp"


State::MainMenu::MainMenu(gameCore& gc) :State_Base(gc), _standardFont(*Services::fonts.loadFont("test"))
{
	std::filesystem::create_directories(HI2::getSavesPath());

	HI2::Texture& bg = *Services::graphics.loadTexture("bg");


	_uiScene.addGadget(std::make_shared<imageView>(point2D{ 0,0 }, point2D{ 1280,720 }, bg));
	_uiScene.addGadget(std::make_shared<textView>(point2D{ HI2::getScreenWidth() / 2 - 110,50 }, point2D{ 220,50 }, "Z5", _standardFont, 200, HI2::Color::White));

	createMainPanel();

	createContinuePanel();

	createNewGamePanel();

	HI2::setBackgroundColor(HI2::Color(0, 0, 0, 255));
}

void State::MainMenu::input(double dt) {
	if (HI2::getKeysDown() & HI2::BUTTON::KEY_ESCAPE) {
		if (_mainPanel.p->isActive())
		{
			_core->popState();
		}
		else if (_continuePanel.p->isActive()) {
			_continuePanel.p->setActive(false);
			_continuePanel.p->setVisible(false);
			_mainPanel.p->setActive(true);
			_mainPanel.p->setVisible(true);

			_uiScene.select(_mainPanel.p);
		}
		else if (_newGamePanel.p->isActive()) {
			_newGamePanel.p->setActive(false);
			_newGamePanel.p->setVisible(false);
			_mainPanel.p->setActive(true);
			_mainPanel.p->setVisible(true);

			_uiScene.select(_mainPanel.p);
		}
	}
}

void State::MainMenu::update(double dt) {
	_uiScene.update(HI2::getKeysDown(), HI2::getKeysUp(), HI2::getKeysHeld(), HI2::getTouchPos(), dt);
	if (_mainPanel.continueButton->isRisingInside())
	{
		regenerateSavesVector();
		_mainPanel.continueButton->update(dt);
		_continuePanel.p->setActive(true);
		_continuePanel.p->setVisible(true);
		_mainPanel.p->setActive(false);
		_mainPanel.p->setVisible(false);

		_uiScene.select(_continuePanel.p);
	}
	if (_mainPanel.newGameButton->isRisingInside())
	{
		_mainPanel.newGameButton->update(dt);
		_newGamePanel.p->setActive(true);
		_newGamePanel.p->setVisible(true);
		_mainPanel.p->setActive(false);
		_mainPanel.p->setVisible(false);

		_newGamePanel.saveName->setText("New Universe");
		_newGamePanel.seed->setText(std::to_string(rand() % 100000));

		_uiScene.select(_newGamePanel.p);
	}
	if (_newGamePanel.start->isRisingInside())
	{
		if (!_newGamePanel.seed->isEmpty() && !_newGamePanel.saveName->isEmpty() && !std::filesystem::exists(HI2::getSavesPath().append(_newGamePanel.saveName->getText()))) {
			_newGamePanel.seed->setHintColor(HI2::Color::LightestGrey);
			_newGamePanel.saveName->setHintColor(HI2::Color::LightestGrey);

			_newGamePanel.start->update(dt);
			_newGamePanel.p->setActive(false);
			_newGamePanel.p->setVisible(false);
			_mainPanel.p->setActive(true);
			_mainPanel.p->setVisible(true);

			_uiScene.select(_mainPanel.p);

			_core->pushState(std::make_unique<State::Playing>(*_core, _newGamePanel.saveName->getText(), std::stoi(_newGamePanel.seed->getText(), nullptr)));

			_newGamePanel.saveName->setText("New Universe");
			_newGamePanel.seed->setText(std::to_string(rand() % 100000));
		}
		else {
			if (_newGamePanel.seed->isEmpty())
				_newGamePanel.seed->setHintColor(HI2::Color::Red);
			if (_newGamePanel.saveName->isEmpty())
				_newGamePanel.saveName->setHintColor(HI2::Color::Red);
		}
	}
	for (save& s : _continuePanel.saves) {
		if (s.startButton->isRisingInside()) {
			s.p->update(dt);
			_continuePanel.p->setActive(false);
			_continuePanel.p->setVisible(false);
			_mainPanel.p->setActive(true);
			_mainPanel.p->setVisible(true);

			_uiScene.select(_mainPanel.p);

			_core->pushState(std::make_unique<State::Playing>(*_core, s.path.filename().string()));
			break;
		}
		if (s.deleteButton->isRisingInside()) {
			s.p->update(dt);
			_continuePanel.p->removeGadget(s.p);
			std::filesystem::remove_all(s.path);
			regenerateSavesVector();
			break;
		}
	}
}

void State::MainMenu::draw(double dt) {
	HI2::startFrame();
	_uiScene.draw();
	HI2::endFrame();
}

void State::MainMenu::regenerateSavesVector()
{
	std::shared_ptr<gadget> last;
	int i = 0;
	HI2::Texture play_on = *Services::graphics.loadTexture("UI/buttons/play_button_on");
	HI2::Texture play_off = *Services::graphics.loadTexture("UI/buttons/play_button_off");
	HI2::Texture delete_on = *Services::graphics.loadTexture("UI/buttons/delete_button_on");
	HI2::Texture delete_off = *Services::graphics.loadTexture("UI/buttons/delete_button_off");
	_continuePanel.saves.clear();
	for (auto& s : std::filesystem::directory_iterator(HI2::getSavesPath())) {
		save savePanel;
		savePanel.p = std::make_shared<basicPanel>(point2D{ 32,32 + 180 * i++ }, point2D{ 936,160 }, HI2::Color::LightestGrey);
		savePanel.path = s.path();
		savePanel.p->addGadget(std::make_shared<textView>(point2D{ 32,32 }, point2D{ 808,32 }, savePanel.path.filename().string(), _standardFont, 32, HI2::Color::Black, HI2::Color::White));
		savePanel.startButton = std::make_shared<imagePushButton>(point2D{ 32,96 }, point2D{ 32,32 }, play_off, play_on);
		savePanel.deleteButton = std::make_shared<imagePushButton>(point2D{ 872,32 }, point2D{ 32,32 }, delete_off, delete_on);
		savePanel.p->addGadget(savePanel.startButton);
		savePanel.p->addGadget(savePanel.deleteButton);

		savePanel.startButton->setRight(savePanel.deleteButton.get());
		savePanel.deleteButton->setLeft(savePanel.startButton.get());

		if (last != nullptr) {
			last->setDown(savePanel.p.get());
			savePanel.p->setUp(last.get());
		}
		last = savePanel.p;
		_continuePanel.saves.push_back(savePanel);
		_continuePanel.p->addGadget(savePanel.p);
	}
	if (i > 2)
		_continuePanel.p->setMaxDimensions({ 1000,32 + 180 * i });
}

void State::MainMenu::createMainPanel()
{
	_mainPanel.p = std::make_shared<basicPanel>(point2D{ HI2::getScreenWidth() / 2 - 400 / 2,320 }, point2D{ 400,288 }, HI2::Color::LightestGrey);

	std::shared_ptr<gadget> last;
	HI2::Texture on = *Services::graphics.loadTexture("UI/buttons/blue_button_on");
	HI2::Texture off = *Services::graphics.loadTexture("UI/buttons/blue_button_off");

	_mainPanel.continueButton = std::make_shared<imagePushButton>(point2D{ 32,32 }, point2D{ 32,32 }, off, on, "");
	_mainPanel.p->addGadget(_mainPanel.continueButton);
	last = _mainPanel.continueButton;
	_mainPanel.p->addGadget(std::make_shared<textView>(point2D{ 80,32 }, point2D{ 200,32 }, "Continue", _standardFont, 32, HI2::Color::Black, HI2::Color::White));

	_mainPanel.newGameButton = std::make_shared<imagePushButton>(point2D{ 32,96 }, point2D{ 32,32 }, off, on, "");
	_mainPanel.p->addGadget(_mainPanel.newGameButton);
	last->setDown(_mainPanel.newGameButton.get());
	_mainPanel.newGameButton->setUp(last.get());
	last = _mainPanel.newGameButton;
	_mainPanel.p->addGadget(std::make_shared<textView>(point2D{ 80,96 }, point2D{ 200,32 }, "New Game", _standardFont, 32, HI2::Color::Black, HI2::Color::White));

	_mainPanel.mapEditorButton = std::make_shared<imagePushButton>(point2D{ 32,160 }, point2D{ 32,32 }, off, on, "");
	_mainPanel.p->addGadget(_mainPanel.mapEditorButton);
	last->setDown(_mainPanel.mapEditorButton.get());
	_mainPanel.mapEditorButton->setUp(last.get());
	last = _mainPanel.mapEditorButton;
	_mainPanel.p->addGadget(std::make_shared<textView>(point2D{ 80,160 }, point2D{ 200,32 }, "Map editor", _standardFont, 32, HI2::Color::Black, HI2::Color::White));

	_mainPanel.optionsButton = std::make_shared<imagePushButton>(point2D{ 32,224 }, point2D{ 32,32 }, off, on, "");
	_mainPanel.p->addGadget(_mainPanel.optionsButton);
	last->setDown(_mainPanel.optionsButton.get());
	_mainPanel.optionsButton->setUp(last.get());
	last = _mainPanel.optionsButton;
	_mainPanel.p->addGadget(std::make_shared<textView>(point2D{ 80,224 }, point2D{ 200,32 }, "Options", _standardFont, 32, HI2::Color::Black, HI2::Color::White));

	_uiScene.addGadget(_mainPanel.p);
}

void State::MainMenu::createNewGamePanel()
{
	_newGamePanel.p = std::make_shared<basicPanel>(point2D{ HI2::getScreenWidth() / 2 - 500 / 2,250 }, point2D{ 500,270 }, HI2::Color::LightestGrey);
	_newGamePanel.p->setActive(false);
	_newGamePanel.p->setVisible(false);
	std::shared_ptr<gadget> last;
	_uiScene.addGadget(_newGamePanel.p);

	_newGamePanel.saveName = std::make_shared<basicTextEntry>(point2D{ 32,32 }, point2D{ _newGamePanel.p->getSize().x - 64,32 }, _standardFont, 32, "New Universe", "Save Name", HI2::Color::White, HI2::Color::Black, HI2::Color::LightGrey);
	_newGamePanel.p->addGadget(_newGamePanel.saveName);
	last = _newGamePanel.saveName;

	_newGamePanel.seed = std::make_shared<basicTextEntry>(point2D{ 32,96 }, point2D{ 120,32 }, _standardFont, 32, std::to_string(rand() % 100000), "", HI2::Color::White, HI2::Color::Black, HI2::Color::LightGrey);
	_newGamePanel.p->addGadget(_newGamePanel.seed);
	last->setDown(_newGamePanel.seed.get());
	_newGamePanel.seed->setUp(last.get());
	last = _newGamePanel.seed;
	_newGamePanel.p->addGadget(std::make_shared<textView>(point2D{ 165,96 }, point2D{ 120,32 }, "Seed", _standardFont, 32, HI2::Color::Black));

	_newGamePanel.debug = std::make_shared<imageToggleButton>(point2D{ 30,160 }, point2D{ 32,32 },*Services::graphics.loadTexture("UI/buttons/toggle_off"),*Services::graphics.loadTexture("UI/buttons/toggle_on"));
	_newGamePanel.p->addGadget(_newGamePanel.debug);
	last->setDown(_newGamePanel.debug.get());
	_newGamePanel.debug->setUp(last.get());
	last = _newGamePanel.debug;
	_newGamePanel.p->addGadget(std::make_shared<textView>(point2D{ 75,160 }, point2D{ 120,32 }, "DEBUG", _standardFont, 32, HI2::Color::Black));

	_newGamePanel.start = std::make_shared<imagePushButton>(point2D{ _newGamePanel.p->getSize().x / 2 - 16,210 }, point2D{ 32,32 },*Services::graphics.loadTexture("UI/buttons/play_button_off"),*Services::graphics.loadTexture("UI/buttons/play_button_on"));
	_newGamePanel.p->addGadget(_newGamePanel.start);
	last->setDown(_newGamePanel.start.get());
	_newGamePanel.start->setUp(last.get());
	last = _newGamePanel.start;
}

void State::MainMenu::createContinuePanel()
{
	_continuePanel.p = std::make_shared<basicScrollablePanel>(point2D{ HI2::getScreenWidth() / 2 - 1000 / 2,250 }, point2D{ 1000,400 }, point2D{ 1000,400 }, HI2::Color::LightestGrey);
	_continuePanel.p->setActive(false);
	_continuePanel.p->setVisible(false);

	_uiScene.addGadget(_continuePanel.p);
}
