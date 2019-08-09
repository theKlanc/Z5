#include "states/state_demo.hpp"
#include <iostream>
#include "gameCore.hpp"
#include "HardwareInterface/HardwareInterface.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_audio.h"
#include "SDL2/SDL_mixer.h"

State::Demo::Demo(gameCore& c): State_Base(c){
	pixelSpd.x=1;
	pixelSpd.y=1;
	pixelPos.x=60;
	pixelPos.y=60;
	graphics g = _core->getGraphics();
	texture=g.loadTexture("test");
	font= HI2::Font(HI2::getDataPath().string()+"fonts/test.ttf");
	effect= HI2::Audio(HI2::getDataPath().string()+"sounds/sfx/oof.mp3",false,1);
	//--


    // Load sound file to use
    // Sound from https://freesound.org/people/jens.enk/sounds/434610/

	//---
}

State::Demo::~Demo(){
	font.clean();
	effect.clean();
}

void State::Demo::input() {

	int held = HI2::getKeysHeld();
	if(held & HI2::BUTTON::KEY_MINUS){
		pixelSpd.x=0;
		pixelSpd.y=0;
	}

	if(held & HI2::BUTTON::KEY_UP){
		pixelSpd.y-=1;
	}
	if(held & HI2::BUTTON::KEY_DOWN){
		pixelSpd.y+=1;
	}
	if(held & HI2::BUTTON::KEY_LEFT){
		pixelSpd.x-=1;
	}
	if(held & HI2::BUTTON::KEY_RIGHT){
		pixelSpd.x+=1;
	}
	if(held & HI2::BUTTON::KEY_PLUS){
		_core->quit();
	}
	if(held & HI2::BUTTON::KEY_A){
		done=true;
	}
	if(held & HI2::BUTTON::KEY_B){
		HI2::playSound(effect);
		std::cout<<"Played Sound"<<std::endl;
	}
}

void State::Demo::update(float dt) {
	pixelPos.x+=pixelSpd.x;
	pixelPos.y+=pixelSpd.y;
	//bounds checks
	if(pixelPos.x < 0){
		pixelSpd.x=pixelSpd.x*-1;
		pixelPos.x=pixelPos.x*-1;
	}
	if(pixelPos.y < 0){
		pixelSpd.y=pixelSpd.y*-1;
		pixelPos.y=pixelPos.y*-1;
	}
	if(pixelPos.x > HI2::getScreenWidth()){
		pixelSpd.x=pixelSpd.x*-1;
		pixelPos.x=(pixelPos.x-HI2::getScreenWidth())*-1 + HI2::getScreenWidth();
	}
	if(pixelPos.y > HI2::getScreenHeight()){
		pixelSpd.y=pixelSpd.y*-1;
		pixelPos.y=(pixelPos.y-HI2::getScreenHeight())*-1 + HI2::getScreenHeight();
	}

}

void State::Demo::draw() {
	HI2::startFrame();
	if(texture != nullptr)
		HI2::drawTexture(*texture,0,0,2);
	HI2::drawText(font,"OOF",point2D{0,0},1,RGBA8(0,255,0,255));
	HI2::drawRectangle(pixelPos,40,40,RGBA8(255,255,255,255));
	HI2::endFrame();
}
