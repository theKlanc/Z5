#pragma once
#include "UI/gadget.hpp"

class textEntry : public gadget{
	public:
		textEntry(point2D pos, point2D size, HI2::Font font, int textSize, std::string text, std::string hint, HI2::Color textColor = HI2::Color::Black, HI2::Color hintColor = HI2::Color::Grey, std::string name = "");
		void draw(point2D offset);
		void update(const unsigned long long& down,const unsigned long long& up,const unsigned long long& held, const point2D& mouse, double dt);
		void setHint(std::string hint);
		void setText(std::string text);
		std::string getText();


	private:
		int _textSize;
		HI2::Font _font;
		std::string _text;
		std::string _hint;
		HI2::Color _textColor;
		HI2::Color _hintColor;
		bool _writing = false;

};
