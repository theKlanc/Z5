#pragma once
#include "UI/gadget.hpp"

class button : public gadget{
	public:
		button(point2D pos, point2D size, std::string s = "");
		void draw(point2D offset) override;

		void update(const unsigned long long &down, const unsigned long long &up, const unsigned long long &held, const point2D &mouse, const double &dt) override;
		void update(const double &dt) override;

		bool isPressed();
		bool isPressing();
		bool isRising();
	private:
		bool _pressed = false;
		bool _oldPressed = false;
};