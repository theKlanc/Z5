#include "UI/gadgets/basicScrollablePanel.hpp"


basicScrollablePanel::basicScrollablePanel(point2D pos, point2D size, int maxHeight, HI2::Color c, std::string s) : scrollablePanel::scrollablePanel(pos,size,maxHeight,s)
{
	_color = c;
}

void basicScrollablePanel::draw(point2D offset){
	HI2::drawRectangle(offset + _position,_size.x,_size.y,_color);
	scrollablePanel::draw(offset);
}

