#pragma once
#include "UI/gadget.hpp"

class textView : public gadget {
public:
	textView(point2D pos, point2D size, std::string text, HI2::Font font, int textSize, HI2::Color color, HI2::Color bgColor = HI2::Color::Transparent, std::string name = "");
	void draw(point2D offset) override;
	void setText(std::string text);
	std::string getText();

private:
	int _textSize;
	std::string _text;
	HI2::Font _font;
	HI2::Color _color;
	HI2::Color _bgColor;
};
