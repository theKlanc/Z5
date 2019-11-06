#pragma once
#include "UI/gadget.hpp"

class imageView : public gadget{
	public:
		imageView(point2D pos, point2D size, HI2::Texture image, std::string s = "");
		void draw(point2D offset) override;
	private:
		HI2::Texture _texture;
};
