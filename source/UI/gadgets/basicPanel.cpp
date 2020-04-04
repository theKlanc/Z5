#include "UI/gadgets/basicPanel.hpp"
#include "HI2.hpp"

basicPanel::basicPanel(point2D pos, point2D size, HI2::Color c, std::string s) : panel(pos,size,s)
{
	_color = c;
}

void basicPanel::draw(point2D offset)
{
	HI2::drawRectangle(offset + _position,_size.x,_size.y,_color);
	panel::draw(offset);
}
