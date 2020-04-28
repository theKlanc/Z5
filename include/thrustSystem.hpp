#pragma once
#include "fuel.hpp"
#include "thruster.hpp"
#include "fuelContainer.hpp"

class thrustSystem
{
public:
	thrustSystem();
	void addThruster(thruster t);
	thruster removeThruster(const thruster& t);
private:
	std::vector<std::unique_ptr<fuelContainer>> _containers;
	int _currentContainerIndex = -1; //[REDACTED] style baby!
	std::vector<std::unique_ptr<thruster>> _thrusters;
};