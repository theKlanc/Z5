#pragma once
#include "states/state_base.hpp"
#include <memory>
#include <stack>
#include <queue>

class gameCore {

  public:
	gameCore();
	~gameCore();
	void pushState(std::unique_ptr<State::State_Base> state);
	void popState(int n = 1);
	void gameLoop();
	void quit();
#ifdef __EMSCRIPTEN__
    void loop();
#endif

  private:
	void processStates();
	int _pop;
    std::chrono::time_point<std::chrono::high_resolution_clock> lastTick;
	std::stack<std::unique_ptr<State::State_Base>> states;
	std::stack<std::unique_ptr<State::State_Base>> pushStates;
	bool _exit;
};
