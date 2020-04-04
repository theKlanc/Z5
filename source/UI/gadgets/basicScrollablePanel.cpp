#include "UI/gadgets/basicScrollablePanel.hpp"


basicScrollablePanel::basicScrollablePanel(point2D pos, point2D size, point2D maxDimensions, HI2::Color c, std::string s) : scrollablePanel::scrollablePanel(pos,size,maxDimensions,s)
{
	_color = c;
}

void basicScrollablePanel::_draw_internal(){
	HI2::drawRectangle(point2D{0,0},_size.x,_size.y,_color);
	scrollablePanel::_draw_internal();
}

