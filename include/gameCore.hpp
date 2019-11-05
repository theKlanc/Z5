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
	void startGameLoop();
	void quit();

	graphics& getGraphics();
  private:
	graphics _graphicsObj;
	void processStates();
	int _pop;
	std::stack<std::unique_ptr<State::State_Base>> states;
	std::stack<std::unique_ptr<State::State_Base>> pushStates;
	bool _exit;
};
