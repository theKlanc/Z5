#include "UI/gadgets/scrollablePanel.hpp"
#include <iostream>
scrollablePanel::scrollablePanel(point2D pos, point2D size, int maxHeight, std::string s) : panel(pos,size,s)
{
	_totalHeight = maxHeight;
}

void scrollablePanel::draw(point2D offset)
{
	for(std::shared_ptr<gadget> g : _gadgets){
		if(g->isVisible() && g->isCompletelyRenderable(offset+point2D{0,_offset},_size))
		{
			g->draw(offset + _position+point2D{0,_offset});
		}
	}
	if(_selected != nullptr && _selected->isVisible() && _selected->isCompletelyRenderable(offset+point2D{0,_offset},_size)){
		HI2::drawEmptyRectangle(_selected->getPosition()+_position+offset+point2D{0,_offset},_selected->getSize().x,_selected->getSize().y,HI2::Color::Blue);
	}
}

void scrollablePanel::update(const double &dt)
{
	panel::update(dt);
}

void scrollablePanel::update(const unsigned long long &down, const unsigned long long &up, const unsigned long long &held, const point2D &mouse, const double &dt)
{
	point2D relativeMouse;
	relativeMouse = mouse-_position;
	bool emptyTouch = false;

	if(down & HI2::BUTTON::TOUCH)
	{
		emptyTouch = true;
		for(std::shared_ptr<gadget> g : _gadgets){
			if(g->touched(point2D{relativeMouse.x,relativeMouse.y-_offset}) && g->isSelectable())
			{
				_selected=g.get();
				emptyTouch = false;
			}
		}
	}
	if(emptyTouch){
		_wasDragging=true;
		_lastTouch=relativeMouse.y;
	}
	if(_wasDragging){
		_offset+=relativeMouse.y-_lastTouch;
		_lastTouch=relativeMouse.y;
	}
	if(_wasDragging && ! (held & HI2::BUTTON::TOUCH && touched(mouse))){
		_wasDragging=false;
	}
	if(_selected != nullptr){
		_selected->update(down,up,held,point2D{relativeMouse.x,relativeMouse.y-_offset},dt);
	}
	if(_offset>0)
		_offset=0;
	if(_offset < -1*(_totalHeight-_size.y))
		_offset= -1*(_totalHeight-_size.y);
	std::cout << _offset << std::endl;
}
