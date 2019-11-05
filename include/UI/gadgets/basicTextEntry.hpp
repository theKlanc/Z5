#pragma once
#include "UI/gadgets/textEntry.hpp"

class basicTextEntry : public textEntry{
public:
	basicTextEntry(point2D pos, point2D size, HI2::Font font, int textSize, std::string text, std::string hint, HI2::Color bgColor, HI2::Color textColor = HI2::Color::Black, HI2::Color hintColor = HI2::Color::Grey, std::string name = "");
	void draw(point2D offset) override;
private:
	HI2::Color _bgColor;
};
