#pragma once
#include "UI/gadgets/scrollablePanel.hpp"


class basicScrollablePanel : public scrollablePanel
{
public:
	basicScrollablePanel(point2D pos, point2D size, point2D maxDimensions, HI2::Color c, std::string s = "");
private:
	HI2::Color _color;
protected:
	void _draw_internal() override;
};

