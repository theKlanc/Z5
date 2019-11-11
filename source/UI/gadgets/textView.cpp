#include "UI/gadgets/textView.hpp"

textView::textView(point2D pos, point2D size, std::string text, HI2::Font font, int textSize, HI2::Color color, HI2::Color bgColor, std::string name)
{
	_position = pos;
	_size = size;
	_font = font;
	_textSize = textSize;
	_text = text;
	_color = color;
	_name = name;
	_selectable = false;
	_bgColor = bgColor;
}

void textView::draw(point2D offset)
{
	HI2::drawRectangle(_position + offset, _size.x, _size.y, _bgColor);
	HI2::drawText(_font, _text, _position + offset, _textSize, _color);
}

void textView::setText(std::string text)
{
	_text = text;
}

std::string textView::getText()
{
	return _text;
}
