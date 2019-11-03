#include "UI/gadgets/button.hpp"

button::button(point2D pos, point2D size, std::string s)
{
	_position = pos;
	_size = size;
	_name = s;
}

void button::draw(point2D offset)
{
	HI2::drawRectangle(_position+offset,_size.x,_size.y,(_pressed?HI2::Color::Green:HI2::Color::Red));
}

void button::update(const unsigned long long &down, const unsigned long long &up, const unsigned long long &held, const point2D &mouse, const double &dt)
{
	_pressed = (held & HI2::BUTTON::KEY_ACCEPT) || (touched(mouse) && (held & HI2::BUTTON::TOUCH));
}

void button::update(const double &dt)
{
	_oldPressed=_pressed;
	_pressed=false;
}

bool button::isPressed()
{
	return _pressed;
}

bool button::isPressing()
{
	return !_oldPressed && _pressed;
}

bool button::isRising()
{
	return _oldPressed && !_pressed;
}
