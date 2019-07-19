#pragma once
#include "state_base.hpp"

namespace State {
	class Playing : public State_Base {
	  public:
		Playing();
		Playing(gameCore &gc);

		void input() override;
		void update(float dt) override;
		void draw() override;

	  private:
	};
} // namespace State
