#pragma once
#include "state_base.hpp"
#include "HardwareInterface/HardwareInterface.hpp"

namespace State {
	class Demo : public State_Base {
	  public:
		Demo(gameCore& c);
		~Demo() override;

		void input(double dt) override;
		void update(double dt) override;
		void draw(double dt) override;

	  private:
		sprite* s;
		HI2::Font* font;
		HI2::Audio* effect;
		point2Dd pixelPos;
		point2Dd pixelSpd;
		bool done=false;
	};
} // namespace State
