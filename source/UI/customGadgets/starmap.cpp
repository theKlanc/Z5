#include "UI/customGadgets/starmap.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <cstdlib>

starmap::starmap(point2D pos, point2D size, universeNode* universe, universeNode* parent,std::string s)
{
	init(pos,size,s);
	_selectable = true;
	_universe = universe;
	_selected = parent;
	_scale = 1/(_selected->getPosition().magnitude()/(sqrt(pow(_size.x,2)+pow(_size.x,2))));
}

void starmap::_draw_internal()
{
	HI2::drawRectangle(point2D{0,0},_size.x,_size.y,HI2::Color::Black);

	drawNodes();

	if(_hovered){
		//We should probably cull the orbit lines, but it doesn't matter in the slightest
		HI2::drawLines(calculateOrbit(_hovered,1.0F/config::physicsHz,100),HI2::Color::Red);
		drawNodeRing(_hovered,HI2::Color::White);
		drawInfo(_hovered,true);
	}

	else{
		HI2::drawLines(calculateOrbit(_selected,1.0F/config::physicsHz,100),HI2::Color::Red);
		drawNodeRing(_selected,flashingCycle>0.5?HI2::Color::Green:HI2::Color::LightGrey);
		drawInfo(_selected,true);
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
		if(&node == _selected)
			continue;
		int nodeSize = node.getDiameter()*_scale;
		if(nodeSize < 2)
			nodeSize=2;

		point2D nodeScreenPos = translateToDisplayCoord(node.getPosition(),node.getParent());
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

void starmap::drawNodes() const
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
		point2D drawPosition = translateToDisplayCoord(node.getPosition(),node.getParent()) - point2D{(int)(node.getDiameter()*_scale/2),(int)(node.getDiameter()*_scale/2)};
		if(isSquareVisible(drawPosition,nodeSize))
			HI2::drawRectangle(drawPosition,nodeSize,nodeSize,node.getMainColor());
	}
}

void starmap::drawNodeRing(universeNode *node, HI2::Color color) const
{
	int nodeSize = node->getDiameter()*_scale;
	if(nodeSize < 2)
		nodeSize=2;
	nodeSize+=6;
	point2D drawPos = point2D{-3,-3} + translateToDisplayCoord(node->getPosition(),node->getParent()) - point2D{(int)(node->getDiameter()*_scale/2),(int)(node->getDiameter()*_scale/2)};
	if(isSquareVisible(drawPos,nodeSize))
		HI2::drawEmptyRectangle(drawPos,nodeSize,nodeSize,color);
}

void starmap::drawInfo(universeNode *node, bool fullInfo) const
{
	HI2::Font font = *Services::fonts.getFont("lemon");
	HI2::drawText(font,node->getName(), point2D{0,0},30,HI2::Color::White);
	HI2::drawText(font,std::to_string(node->getID()), point2D{0,30},30,HI2::Color::White);
	if(fullInfo){
		HI2::drawText(font,"pos: " + std::to_string(node->getPosition().x) + " " +std::to_string(node->getPosition().y) + " " +std::to_string(node->getPosition().z),point2D{0,60},30,HI2::Color::White);
		HI2::drawText(font,"vel: " + std::to_string(node->getVelocity().x) + " " +std::to_string(node->getVelocity().y) + " " +std::to_string(node->getVelocity().z),point2D{0,90},30,HI2::Color::White);
		HI2::drawText(font,"velMagnitude: " + std::to_string(node->getVelocity().magnitude()),point2D{0,120},30,HI2::Color::White);
		HI2::drawText(font,(node->isActive()?"O.O":"zZZzZ"),point2D{0,150},30,HI2::Color::White);
	}
}

point2D starmap::translateToDisplayCoord(fdd position, universeNode *parent) const
{
	fdd relativePosition;
	if(parent == nullptr){
		relativePosition = _selected->getLocalPos(position,_universe);
	}
	else{
		relativePosition = _selected->getLocalPos(position,parent);
	}
	relativePosition*=_scale;
	return point2D{(int)(mapOffset.x * _scale), (int)(mapOffset.y * _scale)} + relativePosition.getPoint2D() + (_size/2);
}

std::vector<point2D> starmap::calculateOrbit(universeNode *node, double timeStep, int pointCount) const
{
	std::vector<point2D> points;
	//timeStep = 600000;
	timeStep *= 1000;
	const int stepsPerPoint = 20000;
	if(node->getParent()){
		points.push_back(translateToDisplayCoord(node->getPosition(),node->getParent()));
		point2D* lastPoint = &points[0];

		fdd position = node->getPosition();
		fdd velocity = node->getVelocity();
		fdd deltaPos;
		int steps = 0;
		int asdf = 0;
		while(points.size()<pointCount && steps < 10000){
			deltaPos += velocity*timeStep;
			steps++;
			velocity +=	node->getParent()->getGravityAcceleration(position,node->getMass())*timeStep;
			if( std::abs(log2(position.magnitude()) - log2(deltaPos.magnitude())) < std::numeric_limits<double>::digits-30){
				position += deltaPos;
				//points.push_back(translateToDisplayCoord(position,node->getParent()));
				deltaPos = fdd();
				//IC("steps for " + node->getName() + " " + std::to_string(steps));
				steps = 0;
				asdf++;
				if(asdf >= stepsPerPoint){
					points.push_back(translateToDisplayCoord(position,node->getParent()));
					asdf = 0;
				}
			}
		}
	}
	return points;
}

bool starmap::isSquareVisible(point2D pos, int size) const
{
	return (0 < pos.x+size && _size.x > pos.x && 0 < pos.y+size && _size.y > pos.y );
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
