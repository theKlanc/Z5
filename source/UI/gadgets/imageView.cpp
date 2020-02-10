#include "UI/gadgets/imageView.hpp"
#include "graphicsManager.hpp"

imageView::imageView(point2D pos, point2D size, sprite image, std::string s)
{
	_position = pos;
	_size = size;
	_selectable = false;
	_sprite = image;
	_name = s;
}

void imageView::draw(point2D offset)
{
	HI2::drawTexture(*_sprite.getTexture(),_position.x+offset.x,_position.y+offset.y);
}
