#include "UI/gadgets/textEntry.hpp"
#include "utils.hpp"

textEntry::textEntry(point2D pos, point2D size, HI2::Font font, int textSize, std::string text, std::string hint, HI2::Color textColor, HI2::Color hintColor, std::string name)
{
	_position = pos;
	_size = size;
	_font = font;
	_textSize = textSize;
	_text = text;
	_hint = hint;
	_textColor=textColor;
	_hintColor=hintColor;
	_name = name;
	_writing = false;
	_selectable = true;

}

void textEntry::update(const unsigned long long &down, const unsigned long long &up, const unsigned long long &held, const point2D &mouse, const double& dt)
{
	if(down & HI2::BUTTON::TOUCH && touched(mouse)){
		_writing = true;
	}
	if(down & HI2::BUTTON::KEY_ACCEPT){
		_writing = !_writing;
	}
	if(down & HI2::BUTTON::KEY_CANCEL){
		_writing = false;
	}
	if(_writing){
		_text = utils::getString(down,_text);
	}
}

void textEntry::draw(point2D offset)
{
	if(_text!="")
		HI2::drawText(_font,_text,_position+offset,_textSize,_textColor);
	else
		HI2::drawText(_font,_hint,_position+offset,_textSize,_hintColor);
}

void textEntry::setHint(std::string hint)
{
	_hint = hint;
}

void textEntry::setText(std::string text)
{
	_text = text;
}

std::string textEntry::getText()
{
	return _text;
}
