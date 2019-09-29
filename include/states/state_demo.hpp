#pragma once
#include "state_base.hpp"
#include "HardwareInterface/HardwareInterface.hpp"

#include "SDL2/SDL_mixer.h"

namespace State {
	class Demo : public State_Base {
	  public:
		Demo(gameCore& c);
		~Demo() override;

		void input(double dt) override;
		void update(double dt) override;
		void draw(double dt) override;

	  private:
		HI2::Texture* texture;
		HI2::Font font;
		HI2::Audio effect;
		Mix_Music *audio;
		point2D pixelPos;
		point2D pixelSpd;
		bool done=false;
	};
} // namespace State
