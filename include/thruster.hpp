#pragma once
#include "fuel.hpp"
#include "HI2.hpp"
#include "json.hpp"
#include "fdd.hpp"
#include "block.hpp"

class universeNode;

class thruster
{
public:
	thruster(){}
	thruster(const thruster &t, fdd pos);
	std::tuple<point3Dd,point3Dd> getThrustVector(double consumedMass, double dt); //[thrust vector in newtons,position]
	double getTargetThrust();
	void setParent(universeNode* u);
	void setTargetThrust(double p);//p is a double from 0.0 to 1.0
	void update(double dt); //lerp towards targetThrust
	double getConsumption(); //returns the current mass consumption per second rate
	bool operator==(const thruster &right) const;
	const fuel* getFuelType() const;
	point3Dd getThrustDirection() const;
	void fix(point3Di dist);

private:
	const fuel *_fuelType = nullptr;
	double _targetThrustPercent = 0;
	double _currentThrustPercent = 0;
	double _maxThrust = 2e6; // maximum thrust magnitude in Newtons
	double _maxConsumption = 500; //fuel consumption at full thrust in kg/s
	point3Dd _position;

	double _thrustAgility = 1.0F/4.0F; //how fast can th
	//rp3d::Quaternion _thrustVectoring;
	point3Dd _thrustDirection = {0,-1,0}; //must be normalized, non rotated thrusters always point downwards, so thrust goes upwards (-y)

	friend void to_json(nlohmann::json& j, const thruster& t);
	friend void from_json(const nlohmann::json& j, thruster& t);

	std::vector<std::pair<metaBlock,point3Di>> _blocks;
	universeNode* _parent;
};

void to_json(nlohmann::json& j, const thruster& t);
void from_json(const nlohmann::json& j, thruster& t);