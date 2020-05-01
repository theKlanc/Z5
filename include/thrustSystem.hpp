#pragma once
#include "fuel.hpp"
#include "thruster.hpp"
#include "fuelContainer.hpp"

struct fuelTypeContainers{
	std::vector<std::unique_ptr<fuelContainer>> containers;
	int currentContainerIndex = -1; //[REDACTED] style baby!
};

class thrustSystem
{
public:
	thrustSystem();
	void addThruster(thruster t);
	thruster removeThruster(const thruster& t);
	void addContainer(fuelContainer fc);
	fuelContainer removeContainer(const fuelContainer& fc);

	void update(double dt);

	double addFuel(const fuel* f, double kg); //Returns the remainder
	double getFuel(const fuel* f, double kg); //Returns the remainder

	std::tuple<point3Dd,point3Dd> getThrust(double dt);

	void setThrustTarget(point3Dd target = {});
private:
	std::unordered_map<unsigned,fuelTypeContainers> _containers;
	std::vector<std::unique_ptr<thruster>> _thrusters;

	friend void to_json(nlohmann::json& j, const thrustSystem& t);
	friend void from_json(const nlohmann::json& j, thrustSystem& t);
};

void to_json(nlohmann::json& j, const fuelTypeContainers& t);
void from_json(const nlohmann::json& j, fuelTypeContainers& t);

void to_json(nlohmann::json& j, const thrustSystem& t);
void from_json(const nlohmann::json& j, thrustSystem& t);