#pragma once
#include "states/state_base.h"
#include <memory>
#include <stack>

class gameCore {

  public:
	gameCore();
	~gameCore();
	void pushState(std::unique_ptr<State::State_Base> state);
	void popState(int n = 1);
	void startGameLoop();
	void quit();

  private:
	void clean();
	int pop;
	std::stack<std::unique_ptr<State::State_Base>> states;
	bool exit;
};
