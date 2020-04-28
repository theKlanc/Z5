#include "thruster.hpp"
#include "jsonTools.hpp"

point3Dd thruster::getThrustVector(double consumedMass, double dt)
{
	double currentWorkloadPercent = consumedMass/(_maxConsumption*dt);
	return _thrustDirection * (_maxThrust * currentWorkloadPercent);
}

double thruster::getTargetThrust()
{
	return _targetThrustPercent;
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

bool thruster::operator==(const thruster &right) const
{
	return _position == right._position;
}

void to_json(nlohmann::json &j, const thruster &t)
{
	j = nlohmann::json{{"fuelID",t._fuelType->ID},{"target",t._targetThrustPercent},{"current",t._currentThrustPercent},{"maxThrust",t._maxThrust},{"consumption",t._maxConsumption},{"agility",t._thrustAgility},{"direction",t._thrustDirection},{"position",t._position}};
}

void from_json(const nlohmann::json &j, thruster &t)
{
	unsigned fueltofind = j.at("fuelID").get<unsigned>();
	t._position = j.at("position").get<fdd>();
	t._fuelType = std::find_if(fuel::fuelList.begin(),fuel::fuelList.end(),[fueltofind](const fuel& f) { return f.ID == fueltofind;}).base();
	t._targetThrustPercent = j.at("target").get<double>();
	t._currentThrustPercent = j.at("current").get<double>();
	t._maxThrust = j.at("maxThrust").get<double>();
	t._maxConsumption = j.at("consumption").get<double>();
	t._thrustAgility = j.at("agility").get<double>();
	t._thrustDirection = j.at("direction").get<point3Dd>();
}
