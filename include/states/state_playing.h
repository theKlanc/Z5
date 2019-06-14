#pragma once
#include "state_base.h"

namespace State {
	class Playing : public State_Base {
	  public:
		Playing();

		void input() override;
		void update(float dt) override;
		void draw() override;

	  private:
	};
} // namespace State
