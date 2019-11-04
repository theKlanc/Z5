#include "UI/gadgets/toggleButton.hpp"
#include "UI/gadgets/pushButton.hpp"

void toggleButton::update(const unsigned long long &down, const unsigned long long &up, const unsigned long long &held, const point2D &mouse, const double &dt)
{
	if((down & HI2::BUTTON::KEY_ACCEPT) || (touched(mouse) && (down & HI2::BUTTON::TOUCH)))
		_pressed = !_pressed;
}

void toggleButton::update(const double &dt)
{
	_oldPressed = _pressed;
}
