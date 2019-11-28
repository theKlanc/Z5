#pragma once
#include "UI/gadgets/panel.hpp"

class scrollablePanel : public panel{
public:
	scrollablePanel(point2D pos, point2D size, point2D maxDimensions, std::string s = "");
	void draw(point2D offset) override;
	void update(const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held, const point2D& mouse, const double& dt) override;
	gadget* getRight() override;
	gadget* getUp() override;
	gadget* getLeft() override;
	gadget* getDown() override;
	void setMaxDimensions(point2D d);
private:
	void correctOffsetBounds();
	void fitOffsetToGadget(gadget* g);

	point2D _offset = {0,0};
	point2D _maxDimensions;
	bool _wasDragging = false;
	point2D _lastTouch;

	const int _scrollMultiplier = 40;
};
