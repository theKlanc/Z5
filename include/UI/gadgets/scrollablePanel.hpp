#pragma once
#include "UI/gadgets/panel.hpp"

class scrollablePanel : public panel{
public:
	scrollablePanel(point2D pos, point2D size, int maxHeight, std::string s = "");
	void draw(point2D offset) override;
	void update(const unsigned long long& down,const unsigned long long& up,const unsigned long long& held, const point2D& mouse, const double& dt) override;
	gadget* getDown() override;
	gadget* getUp() override;
private:
	int _offset = 0;
	int _totalHeight;
	bool _wasDragging = false;
	int _lastTouch;
};
