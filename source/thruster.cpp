#include "thruster.hpp"
#include "jsonTools.hpp"
#include <tuple>

thruster::thruster(const thruster &t, fdd pos)
{
	_fuelType = t._fuelType;
	_maxThrust = t._maxThrust;
	_maxConsumption = t._maxConsumption;
	_thrustAgility = t._thrustAgility;
	_thrustDirection.rotate2D(pos.r);
	_position = pos.getPoint3Dd();
}

std::tuple<point3Dd,point3Dd> thruster::getThrustVector(double consumedMass, double dt)
{
	double currentWorkloadPercent = consumedMass/(_maxConsumption*dt);
	return {_thrustDirection * (_maxThrust * currentWorkloadPercent),_position};
}

double thruster::getTargetThrust()
{
	return _targetThrustPercent;
}


bool thruster::operator==(const thruster &right) const
{
	return _position == right._position; //According to the Pauli exclusion principle, two thrusters cannot coexist in the same exact position, therefore we only need to check for equal positions when comparing
}

const fuel *thruster::getFuelType() const
{
	return _fuelType;
}

point3Dd thruster::getThrustDirection() const
{
	return _thrustDirection;
}

void thruster::setTargetThrust(double p)
{
	if(p<0)
		p=0;
	if(p>1)
		p=1;

	_targetThrustPercent=p;
}

void thruster::update(double dt)
{
	if(_targetThrustPercent != _currentThrustPercent){
		double mobility = dt * _thrustAgility;
		if(std::abs(_currentThrustPercent-_targetThrustPercent) < mobility){
			_currentThrustPercent=_targetThrustPercent;
		}
		else if(_currentThrustPercent < _targetThrustPercent){
			_currentThrustPercent+=mobility;
		}
		else{
			_currentThrustPercent-=mobility;
		}
	}
}

double thruster::getConsumption()
{
	return _currentThrustPercent*_maxConsumption;
}


void to_json(nlohmann::json &j, const thruster &t)
{
	j = nlohmann::json{{"fuelID",t._fuelType->ID},{"target",t._targetThrustPercent},{"current",t._currentThrustPercent},{"maxThrust",t._maxThrust},{"consumption",t._maxConsumption},{"agility",t._thrustAgility},{"direction",t._thrustDirection},{"position",t._position}};
}

void from_json(const nlohmann::json &j, thruster &t)
{
	unsigned fueltofind = j.at("fuelID").get<unsigned>();
	t._position = j.at("position").get<point3Dd>();
	t._fuelType = std::find_if(fuel::fuelList.begin(),fuel::fuelList.end(),[fueltofind](const fuel& f) { return f.ID == fueltofind;}).base();
	t._targetThrustPercent = j.at("target").get<double>();
	t._currentThrustPercent = j.at("current").get<double>();
	t._maxThrust = j.at("maxThrust").get<double>();
	t._maxConsumption = j.at("consumption").get<double>();
	t._thrustAgility = j.at("agility").get<double>();
	t._thrustDirection = j.at("direction").get<point3Dd>();
}
