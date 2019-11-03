#pragma once
#include "HardwareInterface/HardwareInterface.hpp"
namespace utils{
	char getChar(HI2::BUTTON b);
	std::string getString(unsigned long long buttons, std::string str);
}