#pragma once
#include "toggleButton.hpp"
#include <graphicsManager.hpp>

class imageToggleButton : public toggleButton
{
public:
	imageToggleButton(point2D pos, point2D size, sprite off, sprite on, std::string s = ""):toggleButton(pos,size,s),_off(off),_on(on){}
	void draw(point2D offset) override;
	void drawOverlay(point2D offset) override;
private:
	sprite _off;
	sprite _on;
};
