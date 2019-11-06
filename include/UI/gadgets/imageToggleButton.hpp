#pragma once
#include "toggleButton.hpp"

class imageToggleButton : public toggleButton
{
public:
	imageToggleButton(point2D pos, point2D size, HI2::Texture off, HI2::Texture on, std::string s = ""):toggleButton(pos,size,s),_off(off),_on(on){}
	void draw(point2D offset) override;
	void drawOverlay(point2D offset) override;
private:
	HI2::Texture _off;
	HI2::Texture _on;
};
