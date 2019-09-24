#pragma once
#include "graphics.hpp"

class gameCore;

namespace State {
	class State_Base {
	  public:
		State_Base(){
			_core=nullptr;
		}
		State_Base(gameCore& c) {
			_core = &c;
		}
		virtual ~State_Base() {}

		virtual void input(float dt) = 0;
		virtual void update(float dt) = 0;
		virtual void draw(float dt) = 0;

	  protected: // aqui puc posar variables que els estats hagin de compartir
		gameCore* _core;
	};
} // namespace State