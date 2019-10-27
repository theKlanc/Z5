#include "gameCore.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "states/state_playing.hpp"
#include "states/state_demo.hpp"
#include <memory>
#include <iostream>

void gameCore::startGameLoop() {
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTick  = std::chrono::high_resolution_clock::now();
	
	while (HI2::aptMainLoop() && !states.empty() && !_exit) {
		
		std::chrono::time_point<std::chrono::high_resolution_clock> currentTick = std::chrono::high_resolution_clock::now();
		double microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(currentTick - lastTick).count();
		double msOg = microSeconds;
		//if(microSeconds/1000000.0f>(double)(1.0f/29.0f))
		//{
		//	microSeconds=(1.0f/29.0f)*1000000.0f;
		//}
		
		states.top()->input((double)microSeconds/1000000);

		states.top()->update((double)microSeconds/1000000);

		states.top()->draw((double)msOg/1000000);

		
		//std::cout<<"FPS: "<<1/((double)microSeconds/1000000)<<std::endl;
		lastTick = currentTick;
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
	_exit = false;
	_pop = 0;
	// HI2::consoleInit();
	pushState(std::make_unique<State::Playing>(*this,"default",0));
	//pushState(std::make_unique<State::Demo>(*this));
	
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
