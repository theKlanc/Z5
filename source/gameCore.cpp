#include "gameCore.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "states/state_playing.hpp"
#include <memory>

void gameCore::startGameLoop() {

	while (HI2::aptMainLoop() && !states.empty() && !_exit) {

		states.top()->input();

		states.top()->update(0);

		states.top()->draw();
		clean();
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
	pushState(std::make_unique<State::Playing>(*this));
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