#include "states/state_demo.hpp"
#include <iostream>
#include "gameCore.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "services.hpp"
#include "FastNoise/FastNoise.h"

State::Demo::Demo(gameCore& c) : State_Base(c) {
	whiteNoise.SetNoiseType(FastNoise::WhiteNoise);
	simplexNoise.SetNoiseType(FastNoise::SimplexFractal);

}

State::Demo::~Demo() {

}

void State::Demo::input(double dt) {
	auto keys = HI2::getKeysHeld();
	if (keys[HI2::BUTTON::CANCEL]) {
		_core->popState();
	}
	if(keys[HI2::BUTTON::LEFT]){
		camera.x--;
	}
	if(keys[HI2::BUTTON::RIGHT]){
		camera.x++;
	}
	if(keys[HI2::BUTTON::UP]){
		camera.y--;
	}
	if(keys[HI2::BUTTON::DOWN]){
		camera.y++;
	}
}

void State::Demo::update(double dt) {

}

void State::Demo::draw(double dt) {
	HI2::startFrame();
	for(int x = 0; x < HI2::getScreenWidth();x++){
		for(int y = 0;y<HI2::getScreenHeight();y++){
			double density = (simplexNoise.GetNoise(camera.x+x,camera.y+y)+1.0f)/2.0f;
			if(density > maxCutoff)
				density = maxCutoff;
			if(density < minCutoff)
				density = 0;
			if((camera.x+x)%minSpacing!=0 || (camera.y+y)%minSpacing!=0)
				density = 0;
			double noise = (whiteNoise.GetNoise(camera.x+x,camera.y+y)+1.0f)/2.0f;
			HI2::drawPixel({x,y},(noise<density?HI2::Color::Black:HI2::Color::White));
		}
	}
	HI2::endFrame();
}
