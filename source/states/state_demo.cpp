#include "states/state_demo.hpp"
#include <iostream>
#include "gameCore.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "services.hpp"
#include "FastNoise/FastNoise.h"

State::Demo::Demo(gameCore& c) : State_Base(c) {
	whiteNoise.SetNoiseType(FastNoise::WhiteNoise);
	whiteNoiseDisplacementX.SetNoiseType(FastNoise::WhiteNoise);
	whiteNoiseDisplacementY.SetNoiseType(FastNoise::WhiteNoise);
	simplexNoise.SetNoiseType(FastNoise::SimplexFractal);

	whiteNoise.SetSeed(1);
	whiteNoiseDisplacementX.SetSeed(2);
	whiteNoiseDisplacementY.SetSeed(3);
	simplexNoise.SetSeed(4);;
}

State::Demo::~Demo() {

}

void State::Demo::input(double dt) {
	auto keys = HI2::getKeysHeld();
	if (keys[HI2::BUTTON::CANCEL]) {
		_core->popState();
	}
	if(keys[HI2::BUTTON::LEFT]){
		camera.x-=10;
	}
	if(keys[HI2::BUTTON::RIGHT]){
		camera.x+=10;
	}
	if(keys[HI2::BUTTON::UP]){
		camera.y-=10;
	}
	if(keys[HI2::BUTTON::DOWN]){
		camera.y+=10;
	}
	if(keys[HI2::BUTTON::TOUCH]){
		point2D mouse = HI2::getTouchPos();
		maxCutoff = (double)mouse.x / (double)HI2::getScreenWidth();
		minCutoff = (double)mouse.y / (double)HI2::getScreenHeight();
		std::cout << "maxCutoff: " << maxCutoff << std::endl;
		std::cout << "minCutoff: " << minCutoff << std::endl;

	}
	if(keys[HI2::BUTTON::KEY_RIGHTCLICK]){
		point2D mouse = HI2::getTouchPos();
		minSpacing = (double)mouse.x / (double)HI2::getScreenWidth() * 10;
		std::cout << "minSpacing: " << minSpacing << std::endl;
	}
}

void State::Demo::update(double dt) {

}

void State::Demo::draw(double dt) {
	HI2::startFrame();
	for(int x = 0; x < HI2::getScreenWidth();x++){
		for(int y = 0;y<HI2::getScreenHeight();y++){
			point2D displacement = {round(((whiteNoiseDisplacementX.GetNoise(camera.x+x,camera.y+y))*(minSpacing-1)*2)),round(((whiteNoiseDisplacementY.GetNoise(camera.x+x,camera.y+y))*(minSpacing-1)*2))};
			double density = (simplexNoise.GetNoise(camera.x+x,camera.y+y)+1.0f)/2.0f;
			if(density > maxCutoff)
				density = maxCutoff;
			if(density < minCutoff)
				density = 0;
			if((camera.x+x+displacement.x)%(minSpacing+1)!=0 || (camera.y+y+displacement.y)%(minSpacing+1)!=0)
				density = 0;
			double noise = (whiteNoise.GetNoise(camera.x+x,camera.y+y)+1.0f)/2.0f;
			HI2::drawPixel({x,y},(noise<density?HI2::Color::Black:HI2::Color::White));
		}
	}
	HI2::endFrame();
}
