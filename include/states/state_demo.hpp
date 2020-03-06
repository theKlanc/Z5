#pragma once
#include "state_base.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "FastNoise/FastNoise.h"

namespace State {
	class Demo : public State_Base {
	  public:
		Demo(gameCore& c);
		~Demo() override;

		void input(double dt) override;
		void update(double dt) override;
		void draw(double dt) override;

	  private:
		point2D camera;
		FastNoise whiteNoise;
		FastNoise simplexNoise;
		double maxCutoff = 0.8;
		double minCutoff = 0.3;
		int minSpacing = 2;
	};
} // namespace State
