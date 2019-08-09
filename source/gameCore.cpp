#include "gameCore.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "states/state_playing.hpp"
#include "states/state_demo.hpp"
#include <memory>
#include <iostream>

void gameCore::startGameLoop() {
	std::cout << "LMAO";
	while (HI2::aptMainLoop() && !states.empty() && !_exit) {

		states.top()->input();

		states.top()->update(0);

		states.top()->draw();
	}
}

void gameCore::quit() { _exit = true; }

graphics &gameCore::getGraphics()
{
	return _graphicsObj;
}

void gameCore::clean() {
	while (_pop > 0) {
		states.pop();
		_pop--;
	}
}

gameCore::gameCore() {
	HI2::systemInit();
	// HI2::consoleInit();
	pushState(std::make_unique<State::Playing>(*this,"default"));
	_exit = false;
	_pop = 0;
}

gameCore::~gameCore() {
	while (!states.empty())
		states.pop();
	HI2::systemFini();
}

void gameCore::pushState(std::unique_ptr<State::State_Base> state) {
	states.push(std::move(state));
}

void gameCore::popState(int n) { _pop += n; }
