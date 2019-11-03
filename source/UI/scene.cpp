#include "UI/scene.hpp"

scene::scene(std::string name)
{
	_name=name;
}

std::string_view scene::getName()
{
	return _name;
}

void scene::setName(std::string s)
{
	_name = s;
}

void scene::draw()
{
	for(std::shared_ptr<gadget> g : _gadgets){
		if(g->isVisible())
			g->draw({0,0});
	}
}

void scene::update(const unsigned long long& down,const unsigned long long& up,const unsigned long long& held, const point2D& mouse, double dt)
{
	for(std::shared_ptr<gadget> g : _gadgets){
		g->update(dt);
	}


	if(down & HI2::BUTTON::BUTTON_RIGHT){
		if(_selected->getRight() != nullptr)
			_selected = _selected->getRight();
	}
	else if(down & HI2::BUTTON::BUTTON_UP){
		if(_selected->getUp() != nullptr)
			_selected = _selected->getUp();
	}
	else if(down & HI2::BUTTON::BUTTON_LEFT){
		if(_selected->getLeft() != nullptr)
			_selected = _selected->getLeft();
	}
	else if(down & HI2::BUTTON::BUTTON_DOWN){
		if(_selected->getDown() != nullptr)
			_selected = _selected->getDown();
	}


	if(_selected != nullptr){
		_selected->update(down,up,held,mouse,dt);
	}

}

void scene::addGadget(std::shared_ptr<gadget> g)
{
	_gadgets.push_back(g);
	if(_selected == nullptr){
		_selected = g.get();
	}
}
