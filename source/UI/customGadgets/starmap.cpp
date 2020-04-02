#include "UI/customGadgets/starmap.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>

starmap::starmap(point2D pos, point2D size, universeNode* universe, universeNode* parent,std::string s)
{
	_selectable = true;
	_position = pos;
	_size = size;
	_name = s;
	_universe = universe;
	_selected = parent;
}

void starmap::draw(point2D offset)
{
	HI2::drawRectangle(offset + _position,_size.x,_size.y,HI2::Color::Black);
	drawNodes(offset);

	if(_hovered){
		drawNodeRing(offset,_hovered,HI2::Color::White);
		drawInfo(offset,_hovered,false);
	}
	else{
		if(flashingCycle>0.5)
			drawNodeRing(offset,_selected,HI2::Color::Green);
		drawInfo(offset,_selected,true);
	}
}

void starmap::update(const double& dt)
{
	flashingCycle+=dt;
	if(flashingCycle>1)
		flashingCycle=0;
}

universeNode *starmap::getHoveredNode(const point2D &mouse) const
{
	universeNode* closestNode = nullptr;
	int minDistance = 30;
	for(auto& node : *_universe){
		fdd relativePosition;
		if(node.getDepth()==0){
			relativePosition = _selected->getLocalPos(node.getPosition(),_universe);
		}
		else{
			relativePosition = _selected->getLocalPos(node.getPosition(),node.getParent());
		}
		relativePosition*=_scale;
		int nodeSize = node.getDiameter()*_scale;
		if(nodeSize < 2)
			nodeSize=2;
		point2D nodeScreenPos = point2D{(int)(mapOffset.x * _scale), (int)(mapOffset.y * _scale)} + _position + relativePosition.getPoint2D() + (_size/2) - _position;
		point2D diff = nodeScreenPos-mouse;
		int dist = sqrt(pow(diff.x,2) + pow(diff.y,2));

		dist-=nodeSize;
		if(dist < 0)
			dist = 0;
		if(dist <= minDistance + 2 && ((closestNode && node.getDiameter() < closestNode->getDiameter()) || dist < minDistance)){
			minDistance = dist;
			closestNode = &node;
		}
	}
	return closestNode;
}

void starmap::drawNodes(const point2D& offset) const
{
	for(auto& node : *_universe){
		fdd relativePosition;
		if(node.getDepth()==0){
			relativePosition = _selected->getLocalPos(node.getPosition(),_universe);
		}
		else{
			relativePosition = _selected->getLocalPos(node.getPosition(),node.getParent());
		}
		relativePosition*=_scale;
		int nodeSize = node.getDiameter()*_scale;
		if(nodeSize < 2)
			nodeSize=2;
		HI2::drawRectangle(point2D{(int)(mapOffset.x * _scale), (int)(mapOffset.y * _scale)} + _position + offset + relativePosition.getPoint2D() + (_size/2)  - point2D{(int)(node.getDiameter()*_scale/2),(int)(node.getDiameter()*_scale/2)},nodeSize,nodeSize,node.getMainColor());
	}
}

void starmap::drawNodeRing(const point2D& offset,universeNode *node, HI2::Color color)
{
	fdd relativePosition;
	if(node->getDepth()==0){
		relativePosition = _selected->getLocalPos(node->getPosition(),_universe);
	}
	else{
		relativePosition = _selected->getLocalPos(node->getPosition(),node->getParent());
	}
	relativePosition*=_scale;
	int nodeSize = node->getDiameter()*_scale;
	if(nodeSize < 2)
		nodeSize=2;
	nodeSize+=6;
	HI2::drawEmptyRectangle(point2D{(int)(mapOffset.x * _scale)-3, (int)(mapOffset.y * _scale)-3} + _position + offset + relativePosition.getPoint2D() + (_size/2)  - point2D{(int)(node->getDiameter()*_scale/2),(int)(node->getDiameter()*_scale/2)},nodeSize,nodeSize,color);
}

void starmap::drawInfo(const point2D &offset, universeNode *node, bool fullInfo)
{
	HI2::Font font = *Services::fonts.getFont("lemon");
	HI2::drawText(font,node->getName(),offset + _position,20,HI2::Color::White);
}

void starmap::update(const std::bitset<HI2::BUTTON_SIZE>& down, const std::bitset<HI2::BUTTON_SIZE>& up, const std::bitset<HI2::BUTTON_SIZE>& held, const point2D& mouse, const double& dt)
{
	if(down[HI2::BUTTON::KEY_MOUSEWHEEL_UP]){
		_scale*=2;
		mapOffset = mapOffset / 2;
	}
	if(down[HI2::BUTTON::KEY_MOUSEWHEEL_DOWN]){
		_scale/=2;
	}

	_hovered = getHoveredNode(mouse);

	//dragging
	if(held[HI2::BUTTON::KEY_LEFTCLICK]){
		if(!wasHolding){
			lastMousePos = mouse;
		}
		{
			mapOffset = mapOffset + point2Dd(point2Dd(mouse - lastMousePos) * (1/_scale));
			lastMousePos = mouse;
		}
	}
	if(down[HI2::BUTTON::KEY_LEFTCLICK] && _hovered){
		_selected = _hovered;
		mapOffset = point2D();
	}

	wasHolding = held[HI2::BUTTON::KEY_LEFTCLICK];
}
