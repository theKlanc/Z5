#include "UI/gadgets/basicTextEntry.hpp"

basicTextEntry::basicTextEntry(point2D pos, point2D size, HI2::Font font, int textSize, std::string text, std::string hint, HI2::Color bgColor, HI2::Color textColor, HI2::Color hintColor, std::string name):textEntry(pos,size,font,textSize,text,hint,textColor,hintColor,name)
{
	_bgColor=bgColor;
}

void basicTextEntry::_draw_internal()
{
	HI2::drawRectangle(point2D{0,0},_size.x,_size.y,_bgColor);
	textEntry::_draw_internal();
}
