#pragma once
#include "toggleButton.hpp"
#include <graphicsManager.hpp>

class imageToggleButton : public toggleButton
{
public:
	imageToggleButton(point2D pos, point2D size, sprite off, sprite on, std::string s = ""):toggleButton(pos,size,s),_off(off),_on(on){}
private:
	sprite _off;
	sprite _on;

protected:
	void _draw_internal() override;
	void _draw_overlay_internal() override;
};
