#include "UI/gadgets/imageView.hpp"
#include "graphicsManager.hpp"

imageView::imageView(point2D pos, point2D size, sprite image, std::string s)
{
	init(pos,size,s);
	_selectable = false;
	_sprite = image;
}

void imageView::_draw_internal()
{
	HI2::drawTexture(*_sprite.getTexture(),0,0);
}
