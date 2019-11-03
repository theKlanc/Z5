#include "UI/gadget.hpp"

void gadget::draw(point2D offset){}

void gadget::update(const unsigned long long &down, const unsigned long long &up, const unsigned long long &held, const point2D &mouse, const double &dt){}

void gadget::update(const double &dt){}

void gadget::setPosition(point2D pos)
{
	_position = pos;
}

void gadget::setSize(point2D size)
{
	_size = size;
}

point2D gadget::getPosition()
{
	return _position;
}

point2D gadget::getSize()
{
	return _size;
}

std::string_view gadget::getName()
{
	return _name;
}

void gadget::setActive(bool b)
{
	_active=b;
}

void gadget::setVisible(bool b)
{
	_visible=b;
}

void gadget::setSelectable(bool b)
{
	_selectable=b;
}

bool gadget::isVisible()
{
	return _visible;
}

bool gadget::isActive()
{
	return _active;
}

bool gadget::isRenderable()
{
	return (_position.x>=0 && _position.x < HI2::getScreenWidth()) || (_position.y>=0 && _position.y < HI2::getScreenHeight())
			|| (_position.x+_size.x>=0 && _position.x+_size.x < HI2::getScreenWidth()) || (_position.y+_size.y>=0 && _position.y+_size.y < HI2::getScreenHeight());
}

bool gadget::isSelectable()
{
	return _selectable;
}

void gadget::setRight(gadget *g)
{
	_right = g;
}

void gadget::setUp(gadget *g)
{
	_up = g;
}

void gadget::setLeft(gadget *g)
{
	_left = g;
}

void gadget::setDown(gadget *g)
{
	_down = g;
}

gadget *gadget::getRight()
{
	return _right;
}

gadget *gadget::getUp()
{
	return _up;
}

gadget *gadget::getLeft()
{
	return _left;
}

gadget *gadget::getDown()
{
	return _down;
}

bool gadget::touched(point2D touchPosition)
{
	return touchPosition.x > _position.x && touchPosition.y > _position.y &&
			touchPosition.x < (_position.x + _size.x) && touchPosition.y < (_position.y + _size.y);
}
gadget::~gadget(){}
