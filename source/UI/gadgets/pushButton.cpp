#include "UI/gadgets/pushButton.hpp"

pushButton::pushButton(point2D pos, point2D size, std::string s)
{
	_selectable = true;
	_position = pos;
	_size = size;
	_name = s;
}

void pushButton::draw(point2D offset)
{
	HI2::drawRectangle(_position + offset, _size.x, _size.y, (_pressed ? HI2::Color::Green : HI2::Color::Red));
}

void pushButton::update(const std::bitset<HI2::BUTTON_SIZE>& down, const std::bitset<HI2::BUTTON_SIZE>& up, const std::bitset<HI2::BUTTON_SIZE>& held, const point2D& mouse, const double& dt)
{
	_pressed = (held[HI2::BUTTON::ACCEPT]) || (touched(mouse) && (held[HI2::BUTTON::TOUCH]));

	if (isPressing())
		_byMouse = (touched(mouse) && (held[HI2::BUTTON::TOUCH]));

	if (_oldPressed && !_pressed)
	{
		if (_byMouse)
		{
			_rising = (touched(mouse) && (up[HI2::BUTTON::TOUCH]));
		}
		else
		{
			_rising = (up[HI2::BUTTON::ACCEPT]);
		}
		if(_rising)
		{
			if(_clickCallback != nullptr)
			{
				_clickCallback();
			}
		}
	}
}

void pushButton::update(const double& dt)
{
	_oldPressed = _pressed;
	_pressed = false;
	_rising = false;
}

bool pushButton::isPressed()
{
	return _pressed;
}

bool pushButton::isPressing()
{
	return !_oldPressed && _pressed;
}

bool pushButton::isRising()
{
	return _oldPressed && !_pressed;
}

bool pushButton::isRisingInside()
{
	return _rising;
}

void pushButton::setClickCallback(std::function<void()> f)
{
	_clickCallback = f;
}
