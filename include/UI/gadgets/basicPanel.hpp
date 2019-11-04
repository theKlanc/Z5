#pragma once
#include "UI/gadgets/panel.hpp"


class basicPanel : public panel
{
public:
	basicPanel(point2D pos, point2D size,HI2::Color c, std::string s = "");
	void draw(point2D offset) override;
private:
	HI2::Color _color;
};

