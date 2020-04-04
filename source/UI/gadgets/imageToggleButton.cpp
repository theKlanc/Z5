#include "UI/gadgets/imageToggleButton.hpp"

void imageToggleButton::_draw_internal()
{
	HI2::drawTexture(_pressed?*_on.getTexture():*_off.getTexture(),0,0);
}

void imageToggleButton::_draw_overlay_internal()
{
}