#pragma once
#include "UI/gadgets/pushButton.hpp"

class toggleButton : public pushButton{
public:
	toggleButton(point2D pos, point2D size, std::string s = "") : pushButton(pos,size,s){}
	void update(const std::bitset<HI2::BUTTON_SIZE> &down, const std::bitset<HI2::BUTTON_SIZE> &up, const std::bitset<HI2::BUTTON_SIZE> &held, const point2D &mouse, const double &dt) override;
	void update(const double &dt) override;
};
