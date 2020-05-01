#pragma once
#include <bitset>
#include "HardwareInterface/HI2.hpp"
#include <functional>

class controllable
{
public:
	controllable();
	virtual void update(double dt, const std::bitset<HI2::BUTTON_SIZE>& down,const std::bitset<HI2::BUTTON_SIZE>& up,const std::bitset<HI2::BUTTON_SIZE>& held) = 0;
	void setExitCallback(std::function<void()> f);
protected:
	std::function<void()> _exitCallback = nullptr;
};

