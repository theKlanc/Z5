#include "UI/gadgets/panel.hpp"

panel::panel(point2D pos, point2D size, std::string s)
{
	_selectable = true;
	_position=pos;
	_size=size;
	_name=s;
}

void panel::draw(point2D offset)
{
	for(std::shared_ptr<gadget> g : _gadgets){
		if(g->isVisible() && g->isRenderable(offset,_size))
		{
			g->draw(offset + _position);
		}
	}
	if(_selected != nullptr && _selected->isVisible() && _selected->isRenderable(offset,_size)){
		HI2::drawEmptyRectangle(_selected->getPosition()+_position+offset,_selected->getSize().x,_selected->getSize().y,HI2::Color::Blue);
	}
}

void panel::update(const double &dt)
{
	for(std::shared_ptr<gadget> g : _gadgets){
		if(g->isActive())
			g->update(dt);
	}
}

gadget *panel::getRight()
{
	if(_selected != nullptr){
		gadget* temp = _selected->getRight();
		if(temp != nullptr && temp->isSelectable()){
			_selected = temp;
			return this;
		}
		else{
			return gadget::getRight();
		}
	}
	return gadget::getRight();
}

gadget *panel::getUp()
{
	if(_selected != nullptr){
		gadget* temp = _selected->getUp();
		if(temp != nullptr && temp->isSelectable()){
			_selected = temp;
			return this;
		}
		else{
			return gadget::getUp();
		}
	}
	return gadget::getUp();
}

gadget *panel::getLeft()
{
	if(_selected != nullptr){
		gadget* temp = _selected->getLeft();
		if(temp != nullptr && temp->isSelectable()){
			_selected = temp;
			return this;
		}
		else{
			return gadget::getLeft();
		}
	}
	return gadget::getLeft();
}

gadget *panel::getDown()
{
	if(_selected != nullptr){
		gadget* temp = _selected->getDown();
		if(temp != nullptr && temp->isSelectable()){
			_selected = temp;
			return this;
		}
		else{
			return gadget::getDown();
		}
	}
	return gadget::getDown();
}

void panel::update(const unsigned long long &down, const unsigned long long &up, const unsigned long long &held, const point2D &mouse, const double& dt)
{
	point2D relativeMouse;
	relativeMouse = mouse-_position;
	for(std::shared_ptr<gadget> g : _gadgets){
		if(down & HI2::BUTTON::TOUCH && g->touched(relativeMouse) && g->isSelectable())
			_selected=g.get();
	}
	if(_selected != nullptr){
		_selected->update(down,up,held,relativeMouse,dt);
	}
}

void panel::addGadget(std::shared_ptr<gadget> g)
{
	_gadgets.push_back(g);
	if(_selected == nullptr && g->isSelectable()){
		_selected = g.get();
	}
}
