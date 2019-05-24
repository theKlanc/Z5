#include "states/state_demo.h"
#include <iostream>
#include "gameCore.h"
#include "HardwareInterface/HardwareInterface.h"

State::Demo::Demo(gameCore& c): State_Base(c){
	pixelSpd.x=1;
	pixelSpd.y=1;
	pixelPos.x=60;
	pixelPos.y=60;
	texture=HI2::Texture("test.bmp");
	font= HI2::Font("test.ttf");
	//texture=HI2::Texture("test.bmp");
	//font=HI2::Font("test.ttf");
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
	HI2::drawRectangle(pixelPos,40,40,RGBA8(255,255,255,255));
	HI2::drawTexture(texture,0,0);
	HI2::drawText(font,"LMAOOO",point2D{0,0},1,RGBA8(0,255,0,255));
	HI2::endFrame();
}
