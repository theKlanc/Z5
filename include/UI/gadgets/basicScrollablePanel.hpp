#pragma once
#include "UI/gadgets/scrollablePanel.hpp"


class basicScrollablePanel : public scrollablePanel
{
public:
	basicScrollablePanel(point2D pos, point2D size, int maxHeight, HI2::Color c, std::string s = "");
	void draw(point2D offset) override;
private:
	HI2::Color _color;
};

