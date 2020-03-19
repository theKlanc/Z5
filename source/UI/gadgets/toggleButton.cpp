#include "UI/gadgets/toggleButton.hpp"
#include "UI/gadgets/pushButton.hpp"

void toggleButton::update(const std::bitset<HI2::BUTTON_SIZE> &down, const std::bitset<HI2::BUTTON_SIZE> &up, const std::bitset<HI2::BUTTON_SIZE> &held, const point2D &mouse, const double &dt)
{
	if((down[HI2::BUTTON::ACCEPT]) || (touched(mouse) && (down[HI2::BUTTON::TOUCH])))
		_pressed = !_pressed;
}

void toggleButton::update(const double &dt)
{
	_oldPressed = _pressed;
}
