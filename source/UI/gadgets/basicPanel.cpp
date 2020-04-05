#include "UI/gadgets/basicPanel.hpp"
#include "HI2.hpp"

basicPanel::basicPanel(point2D pos, point2D size, HI2::Color c, std::string s) : panel(pos,size,s)
{
	_color = c;
}

void basicPanel::_draw_internal()
{
	HI2::drawRectangle(point2D{0,0},_size.x,_size.y,_color);
	panel::_draw_internal();
}
