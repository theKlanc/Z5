#pragma once
#include "UI/gadget.hpp"
#include "fdd.hpp"

class fddViewer : public gadget
{
public:
	fddViewer(point2D pos, int height, fdd f, fdd lowerBound, fdd upperBound, int marginSize, int puckSize, int textSize, std::string s = "");
	void update(const double& dt) override;
	void setFdd(fdd f = {});
	~fddViewer(){}
	// gadget interface
protected:
	void _draw_internal() override;
	void _draw_overlay_internal() override {}
	fdd _fdd;
	fdd _lowerBound;
	fdd _upperBound;

	int _marginSize;
	int _puckSize;
	int _textSize;
};

