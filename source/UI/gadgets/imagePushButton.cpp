#include "UI/gadgets/imagePushButton.hpp"

void imagePushButton::draw(point2D offset)
{
	point2D finalDrawPos = offset+_position;
	HI2::drawTexture(_pressed?*_on.getTexture():*_off.getTexture(),finalDrawPos.x,finalDrawPos.y);
}

void imagePushButton::drawOverlay(point2D offset)
{
}
