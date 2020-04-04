#pragma once
#include "UI/gadget.hpp"

#include <graphicsManager.hpp>

class imageView : public gadget{
	public:
		imageView(point2D pos, point2D size, sprite image, std::string s = "");
	private:
		sprite _sprite;

protected:
		void _draw_internal() override;
};
