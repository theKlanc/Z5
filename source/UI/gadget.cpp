#include "UI/gadget.hpp"
#include <cmath>

void gadget::draw(point2D offset){}

void gadget::drawOverlay(point2D offset)
{
	HI2::drawEmptyRectangle(_position+offset,_size.x,_size.y,HI2::Color::Blue);
}

void gadget::update(const std::bitset<HI2::BUTTON_SIZE>&down, const std::bitset<HI2::BUTTON_SIZE> &up, const std::bitset<HI2::BUTTON_SIZE> &held, const point2D &mouse, const double &dt){}

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

void gadget::toggle()
{
	_active=!_active;
	_visible=!_visible;
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

bool gadget::isRenderable(point2D offset, point2D parentSize)
{
	point2D finalPos = _position + offset;

	return std::abs(finalPos.x+_size.x + finalPos.x - parentSize.x) <= (finalPos.x+_size.x - finalPos.x + parentSize.x) && std::abs(finalPos.y+_size.y + finalPos.y - parentSize.y) <= (finalPos.y+_size.y - finalPos.y + parentSize.y);

}

bool gadget::isCompletelyRenderable(point2D offset, point2D parentSize)
{
	point2D finalPos = _position + offset;
	point2D corner = finalPos+_size;
	return finalPos.x>0&&finalPos.y>0&&corner.x<parentSize.x&&corner.y<parentSize.y;
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
	return touchPosition.x >= 0 && touchPosition.y >= 0 &&
			touchPosition.x < _size.x && touchPosition.y < _size.y;
}
gadget::~gadget(){}
