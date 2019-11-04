#pragma once
#include "UI/gadgets/pushButton.hpp"

class toggleButton : public pushButton{
public:
	toggleButton(point2D pos, point2D size, std::string s = "") : pushButton(pos,size,s){}
	void update(const unsigned long long &down, const unsigned long long &up, const unsigned long long &held, const point2D &mouse, const double &dt) override;
	void update(const double &dt) override;
};
