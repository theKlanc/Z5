#include "UI/gadgets/scrollablePanel.hpp"
#include <iostream>
scrollablePanel::scrollablePanel(point2D pos, point2D size, point2D maxDimensions, std::string s) : panel(pos,size,s)
{
	_maxDimensions = maxDimensions;
}

void scrollablePanel::draw(point2D offset)
{
	for(std::shared_ptr<gadget> g : _gadgets){
		if(g->isVisible() && g->isCompletelyRenderable(_offset,_size))
		{
			g->draw(offset + _position+_offset);
		}
	}
	if(_selected != nullptr && _selected->isVisible() && _selected->isCompletelyRenderable(offset+_offset,_size)){
		HI2::drawEmptyRectangle(_selected->getPosition()+_position+offset+_offset,_selected->getSize().x,_selected->getSize().y,HI2::Color::Blue);
	}
}

void scrollablePanel::update(const std::bitset<HI2::BUTTON_SIZE> &down, const std::bitset<HI2::BUTTON_SIZE> &up, const std::bitset<HI2::BUTTON_SIZE> &held, const point2D &mouse, const double &dt)
{
	point2D relativeMouse;
	relativeMouse = mouse-_position;
	bool emptyTouch = false;

	if(down[HI2::BUTTON::TOUCH])
	{
		emptyTouch = true;
		for(std::shared_ptr<gadget> g : _gadgets){
			if(g->touched(relativeMouse-_offset) && g->isSelectable())
			{
				_selected=g.get();
				emptyTouch = false;
			}
		}
	}
	if(emptyTouch){
		_wasDragging=true;
		_lastTouch=relativeMouse;
	}
	if(_wasDragging){
		_offset = _offset + relativeMouse - _lastTouch;
		_lastTouch = relativeMouse;
	}
	if(_wasDragging && ! (held[HI2::BUTTON::TOUCH] && touched(mouse))){
		_wasDragging=false;
	}
	if(_selected != nullptr){
		_selected->update(down,up,held,relativeMouse-_offset,dt);
	}

	correctOffsetBounds();
}

gadget *scrollablePanel::getRight()
{
	if(_selected != nullptr){
		gadget* temp = _selected->getRight();
		if(temp != nullptr && temp->isSelectable()){
			_selected = temp;
			fitOffsetToGadget(_selected);
			return this;
		}
		else{
			return gadget::getRight();
		}
	}
	return gadget::getRight();
}

gadget *scrollablePanel::getUp()
{
	if(_selected != nullptr){
		gadget* temp = _selected->getUp();
		if(temp != nullptr && temp->isSelectable()){
			_selected = temp;
			fitOffsetToGadget(_selected);
			return this;
		}
		else{
			return gadget::getUp();
		}
	}
	return gadget::getUp();
}

gadget *scrollablePanel::getLeft()
{
	if(_selected != nullptr){
		gadget* temp = _selected->getLeft();
		if(temp != nullptr && temp->isSelectable()){
			_selected = temp;
			fitOffsetToGadget(_selected);
			return this;
		}
		else{
			return gadget::getLeft();
		}
	}
	return gadget::getLeft();
}

gadget *scrollablePanel::getDown()
{
	if(_selected != nullptr){
		gadget* temp = _selected->getDown();
		if(temp != nullptr && temp->isSelectable()){
			_selected = temp;
			fitOffsetToGadget(_selected);
			return this;
		}
		else{
			return gadget::getDown();
		}
	}
	return gadget::getDown();
}

void scrollablePanel::setMaxDimensions(point2D d)
{
	_maxDimensions=d;
}

void scrollablePanel::correctOffsetBounds()
{
	if(_offset.x>0)
		_offset.x=0;
	if(_offset.x < -1*(_maxDimensions.x-_size.x))
		_offset.x= -1*(_maxDimensions.x-_size.x);
	if(_offset.y>0)
		_offset.y=0;
	if(_offset.y < -1*(_maxDimensions.y-_size.y))
		_offset.y= -1*(_maxDimensions.y-_size.y);
}

void scrollablePanel::fitOffsetToGadget(gadget *g)
{
	if(g->getPosition().y+_offset.y<0){
		_offset.y=-g->getPosition().y+1;
	}
	if(_selected->getPosition().y+_selected->getSize().y+_offset.y>=_size.y){
		_offset.y=-1*(_selected->getPosition().y+_selected->getSize().y-_size.y)-1;
	}
	if(g->getPosition().x+_offset.x<0){
		_offset.x=-g->getPosition().x+1;
	}
	if(_selected->getPosition().x+_selected->getSize().x+_offset.x>=_size.x){
		_offset.x=-1*(_selected->getPosition().x+_selected->getSize().x-_size.x)-1;
	}
}
