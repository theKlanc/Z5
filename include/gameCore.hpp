#pragma once
#include "states/state_base.hpp"
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

	graphics& getGraphics();
  private:
	graphics _graphicsObj;
	void clean();
	int _pop;
	std::stack<std::unique_ptr<State::State_Base>> states;
	bool _exit;
};
