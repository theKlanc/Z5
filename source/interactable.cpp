#include "interactable.hpp"

interactable::interactable()
{

}

void interactable::setPosition(fdd f)
{
	_position = f;
}

void interactable::setParent(universeNode* p)
{
	_parent = p;
}

fdd interactable::getPosition() const
{
	return _position;
}
