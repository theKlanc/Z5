#pragma once
#include "UI/gadget.hpp"
#include <optional>

class textEntry : public gadget{
	public:
		textEntry(point2D pos, point2D size, HI2::Font font, int textSize, std::string text, std::string hint, HI2::Color textColor = HI2::Color::Black, HI2::Color hintColor = HI2::Color::Grey, std::string name = "");
		void draw(point2D offset) override;
		void update(const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held, const point2D& mouse, const double& dt) override;
		void setHint(std::string hint);
		void setText(std::string text);
		void setHintColor(HI2::Color c);
		void setTextColor(HI2::Color c);

		std::string getText();
		bool isEmpty();

		void setCallback(std::function<void(std::string)> callback);
	private:
		int _textSize;
		HI2::Font _font;
		std::string _text;
		std::string _hint;
		HI2::Color _textColor;
		HI2::Color _hintColor;


		std::optional<std::function<void(std::string)>> _callback;
};
