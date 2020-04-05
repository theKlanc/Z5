#include "UI/gadgets/textEntry.hpp"
#include "utils.hpp"

textEntry::textEntry(point2D pos, point2D size, HI2::Font font, int textSize, std::string text, std::string hint, HI2::Color textColor, HI2::Color hintColor, std::string name)
{
	init(pos,size,name);

	_font = font;
	_textSize = textSize;
	_text = text;
	_hint = hint;
	_textColor=textColor;
	_hintColor=hintColor;
	_selectable = true;

}

void textEntry::update(const std::bitset<HI2::BUTTON_SIZE> &down, const std::bitset<HI2::BUTTON_SIZE> &up, const std::bitset<HI2::BUTTON_SIZE> &held, const point2D &mouse, const double& dt)
{
	_text = utils::getString(down,held[HI2::BUTTON::KEY_SHIFT],_text);
	if(down[HI2::BUTTON::ACCEPT]){
		if(_callback)
			_callback.value()(_text);
		_text = "";
		toggle();
	}
}

void textEntry::_draw_internal()
{
	if(_text!="")
		HI2::drawText(_font,_text,point2D{0,0},_textSize,_textColor);
	else
		HI2::drawText(_font,_hint,point2D{0,0},_textSize,_hintColor);
}

void textEntry::setHint(std::string hint)
{
	_hint = hint;
}

void textEntry::setText(std::string text)
{
	_text = text;
}

void textEntry::setHintColor(HI2::Color c)
{
	_hintColor=c;
}

void textEntry::setTextColor(HI2::Color c)
{
	_textColor=c;
}

std::string textEntry::getText()
{
	return _text;
}

bool textEntry::isEmpty()
{
	return _text.empty();
}

void textEntry::setCallback(std::function<void (std::string)> callback)
{
	_callback=callback;
}
