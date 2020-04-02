#pragma once
#include "UI/gadget.hpp"
#include "universeNode.hpp"

class starmap : public gadget {
public:
	starmap(point2D pos, point2D size, universeNode* universe, universeNode* parent,std::string s = "");
	void draw(point2D offset) override;
	void update(const std::bitset<HI2::BUTTON_SIZE>& down, const std::bitset<HI2::BUTTON_SIZE>& up, const std::bitset<HI2::BUTTON_SIZE>& held, const point2D& mouse, const double& dt) override;
	void update(const double& dt) override;
protected:
	universeNode* _universe;
	universeNode* _selected;
	universeNode* _hovered = nullptr;

	double flashingCycle = 0;

	universeNode* getHoveredNode(const point2D& mouse)const;
	void drawNodes(const point2D& offset) const;
	void drawNodeRing(const point2D& offset,universeNode* node, HI2::Color color = HI2::Color::White) const;
	void drawInfo(const point2D& offset,universeNode* node, bool fullInfo = true) const;
	point2D translateToDisplayCoord(fdd position, universeNode* parent) const;
	bool wasHolding = false;
	point2D lastMousePos;
	std::vector<point2D> calculateOrbit(universeNode* node, double timeStep, int stepCount) const;

	point2Dd mapOffset;
	double _scale = 1;
};
