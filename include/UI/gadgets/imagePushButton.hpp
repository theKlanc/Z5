#pragma once
#include "UI/gadgets/pushButton.hpp"

class imagePushButton : public pushButton{
public:
	imagePushButton(point2D pos, point2D size, HI2::Texture off, HI2::Texture on, std::string s = ""):pushButton(pos,size,s),_off(off),_on(on){}
	void draw(point2D offset) override;
	void drawOverlay(point2D offset) override;
private:
	HI2::Texture _off;
	HI2::Texture _on;
};