#include "UI/gadgets/textView.hpp"

textView::textView(point2D pos, point2D size, std::string text, HI2::Font font, int textSize, HI2::Color color, HI2::Color bgColor, std::string name)
{
	init(pos,size,name);

	_font = font;
	_textSize = textSize;
	_text = text;
	_color = color;
	_selectable = false;
	_bgColor = bgColor;
}

void textView::_draw_internal()
{
	HI2::drawRectangle(point2D{0,0}, _size.x, _size.y, _bgColor);
	HI2::drawText(_font, _text, point2D{0,0}, _textSize, _color);
}

void textView::setText(std::string text)
{
	_text = text;
}

std::string textView::getText()
{
	return _text;
}
