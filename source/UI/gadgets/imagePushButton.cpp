#include "UI/gadgets/imagePushButton.hpp"

void imagePushButton::_draw_internal()
{
	HI2::drawTexture(_pressed?*_on.getTexture():*_off.getTexture(),0,0);
}

void imagePushButton::_draw_overlay_internal()
{
}
