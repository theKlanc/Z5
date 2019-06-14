#include "gameCore.h"
#include "states/state_demo.h"
#include <memory>
#include "HardwareInterface/HardwareInterface.h"

void gameCore::startGameLoop() {

	while (HI2::aptMainLoop() && !states.empty() && !exit) {

		states.top()->input();

		states.top()->update(0);

		states.top()->draw();
		clean();

	}
}

void gameCore::quit() { exit = true; }

void gameCore::clean() {
	while (pop > 0) {
		states.pop();
		pop--;
	}
}

gameCore::gameCore() {
	HI2::systemInit();
	//HI2::consoleInit();
	pushState(std::make_unique<State::Demo>(*this));
	exit = false;
	pop = 0;
}

gameCore::~gameCore() {
	while (!states.empty())
		states.pop();
	HI2::systemFini();
}

void gameCore::pushState(std::unique_ptr<State::State_Base> state) {
	states.push(std::move(state));
}

void gameCore::popState(int n) { pop += n; }
