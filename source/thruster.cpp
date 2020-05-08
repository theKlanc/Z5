#include "thruster.hpp"
#include "jsonTools.hpp"
#include <tuple>
#include "universeNode.hpp"
#include "icecream.hpp"

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

void thruster::setParent(universeNode *u)
{
	_parent = u;
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
	double oldPercent = _currentThrustPercent;
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
	if((oldPercent == 0 && _currentThrustPercent != 0) || (oldPercent != 0 && _currentThrustPercent == 0)){
		std::vector<std::pair<metaBlock,point3Di>> newBlocks;
		for(std::pair<metaBlock,point3Di> &m : _blocks){
			newBlocks.push_back(std::make_pair<>(_parent->getBlock(m.second),m.second));
			_parent->setBlock(m.first,m.second);
		}
		_blocks=newBlocks;
	}
}

double thruster::getConsumption()
{
	return _currentThrustPercent*_maxConsumption;
}


void to_json(nlohmann::json &j, const thruster &t)
{
	nlohmann::json jj{};
	for(auto pair : t._blocks){
		jj.push_back(nlohmann::json{{"block",pair.first},{"position",pair.second}});
	}
	j = nlohmann::json{{"fuelID",t._fuelType->ID},{"target",t._targetThrustPercent},{"current",t._currentThrustPercent},{"maxThrust",t._maxThrust},{"consumption",t._maxConsumption},{"agility",t._thrustAgility},{"thrustDirection",t._thrustDirection},{"position",t._position},{"blocks",jj}};
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
	t._thrustDirection = j.at("thrustDirection").get<point3Dd>();
	for(nlohmann::json jj : j.at("blocks")){
		t._blocks.push_back(std::make_pair<>(jj.at("block").get<metaBlock>(),jj.at("position").get<point3Di>()));
	}

}
