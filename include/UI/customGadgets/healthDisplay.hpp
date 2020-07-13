#pragma once
#include "UI/gadget.hpp"
#include "components/health.hpp"

class healthDisplay : public gadget
{
public:
	healthDisplay(point2D pos, health* h,unsigned spacing = 1,double _scaling = 1, std::string s = "");
	void update(const double& dt) override;
	void setHealth(health* h);
	~healthDisplay(){}
	// gadget interface
protected:
	void _draw_internal() override;
	void _draw_overlay_internal() override {}

	HI2::Texture* _empty;
	HI2::Texture* _half;
	HI2::Texture* _full;


	health* _health;
	unsigned _spacing;
	double _scaling = 1;
};
