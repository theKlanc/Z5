#pragma once
#include "state_base.h"
#include "HardwareInterface/HardwareInterface.h"

namespace State {
	class Demo : public State_Base {
	  public:
		Demo(gameCore& c);

		void input() override;
		void update(float dt) override;
		void draw() override;

	  private:
		HI2::Texture texture;
		HI2::Font font;
		point2D pixelPos;
		point2D pixelSpd;
		bool done=false;
	};
} // namespace State
