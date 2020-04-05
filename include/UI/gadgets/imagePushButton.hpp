#pragma once
#include "UI/gadgets/pushButton.hpp"
#include "graphicsManager.hpp"

class imagePushButton : public pushButton{
public:
	imagePushButton(point2D pos, point2D size, sprite off, sprite on, std::string s = ""):pushButton(pos,size,s),_off(off),_on(on){}
private:
	sprite _off;
	sprite _on;

protected:
	void _draw_internal() override;
	void _draw_overlay_internal() override;
};