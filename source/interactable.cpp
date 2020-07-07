#include "interactable.hpp"

interactable::interactable()
{

}

interactable::~interactable(){}

void interactable::setPositions(std::vector<fdd> f)
{
	_positions = f;
}

void interactable::setParent(universeNode* p)
{
	_parent = p;
}

std::vector<fdd> interactable::getPositions() const
{
	return _positions;
}
