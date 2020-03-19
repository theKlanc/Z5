#include "gameCore.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "states/state_playing.hpp"
#include "states/state_demo.hpp"
#include "states/state_main_menu.hpp"
#include <memory>
#include <iostream>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <functional>
void gameCore::loop(){
    if(HI2::aptMainLoop() && !states.empty() && !_exit){
        std::chrono::time_point<std::chrono::high_resolution_clock> currentTick = std::chrono::high_resolution_clock::now();
        double microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(currentTick - lastTick).count();
        double msOg = microSeconds;
        states.top()->input((double)microSeconds/1000000);
        states.top()->update((double)microSeconds/1000000);
        states.top()->draw((double)msOg/1000000);
        lastTick = currentTick;
        processStates();
    }
    else{
        emscripten_cancel_main_loop();
    }
}
static void emloop(void* gc){
    ((gameCore*)gc)->loop();
}
#endif



void gameCore::gameLoop() {
    lastTick  = std::chrono::high_resolution_clock::now();
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(emloop,(void*)this,60,1);
#else
	while (HI2::aptMainLoop() && !states.empty() && !_exit) {
		
		std::chrono::time_point<std::chrono::high_resolution_clock> currentTick = std::chrono::high_resolution_clock::now();
		double microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(currentTick - lastTick).count();
		double msOg = microSeconds;

		
		states.top()->input((double)microSeconds/1000000);

		states.top()->update((double)microSeconds/1000000);

		states.top()->draw((double)msOg/1000000);


  		lastTick = currentTick;
		processStates();
	}
#endif
}

void gameCore::quit() { _exit = true; }

void gameCore::processStates() {
	while (_pop > 0) {
		states.pop();
		_pop--;
	}
	while(!pushStates.empty())
	{
		states.push(std::move(pushStates.top()));
		pushStates.pop();
	}
}

gameCore::gameCore() {
	HI2::consoleInit();
	HI2::systemInit();
	srand(time(NULL));
	_exit = false;
	_pop = 0;
	pushState(std::make_unique<State::MainMenu>(*this));
	//pushState(std::make_unique<State::Demo>(*this));
	processStates();
}

gameCore::~gameCore() {
	while (!states.empty())
	{
		states.pop();
	}
	Services::fonts.freeAllFonts();
	Services::audio.freeAllAudio();
	Services::graphics.freeAllSprites();
	Services::graphics.freeAllTextures();
	HI2::systemFini();
	HI2::consoleFini();
}

void gameCore::pushState(std::unique_ptr<State::State_Base> state) {
	pushStates.push(std::move(state));
}

void gameCore::popState(int n) { _pop += n; }
