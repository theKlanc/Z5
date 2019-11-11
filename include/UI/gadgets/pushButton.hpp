#pragma once
#include "UI/gadget.hpp"

class pushButton : public gadget{
	public:
		pushButton(point2D pos, point2D size, std::string s = "");
		void draw(point2D offset) override;

		void update(const std::bitset<HI2::BUTTON_SIZE>&down, const std::bitset<HI2::BUTTON_SIZE>&up, const std::bitset<HI2::BUTTON_SIZE>&held, const point2D &mouse, const double &dt) override;
		void update(const double &dt) override;

		bool isPressed();
		bool isPressing();
		bool isRising(); // is button rising (even if by dragging mouse out of the button)
		bool isRisingInside(); // is button rising with cursor still inside
	protected:
		bool _pressed = false;
		bool _oldPressed = false;
		bool _rising = false;
		bool _byMouse = false;
};