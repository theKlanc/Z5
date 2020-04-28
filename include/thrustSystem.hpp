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
	void addContainer(fuelContainer fc);
	fuelContainer removeContainer(const fuelContainer& fc);

	double addFuel(const fuel* f, double kg); //Returns the remainder
	double getFuel(const fuel* f, double kg); //Returns the remainder

	std::tuple<point3Dd,point3Dd> getThrust(double dt);
private:
	struct fuelTypeContainers{
		std::vector<std::unique_ptr<fuelContainer>> containers;
		int currentContainerIndex = -1; //[REDACTED] style baby!
	};
	std::unordered_map<unsigned,fuelTypeContainers> _containers;
	std::vector<std::unique_ptr<thruster>> _thrusters;
};