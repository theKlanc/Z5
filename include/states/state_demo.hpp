#pragma once
#include "state_base.hpp"
#include "HI2.hpp"
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
		FastNoise whiteNoiseDisplacementX;
		FastNoise whiteNoiseDisplacementY;
		FastNoise simplexNoise;
		double maxCutoff = 0.9;
		double minCutoff = 0.3;
		int minSpacing = 3;
	};
} // namespace State
