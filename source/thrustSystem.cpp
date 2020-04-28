#include "thrustSystem.hpp"
#include "fuel.hpp"
#include <algorithm>

thrustSystem::thrustSystem()
{
	for(auto f : fuel::fuelList){
		_containers.emplace(f.ID,fuelTypeContainers());
	}
}

void thrustSystem::addThruster(thruster t)
{
	_thrusters.push_back(std::make_unique<thruster>(t));
}

thruster thrustSystem::removeThruster(const thruster &t)
{
	auto it = std::find_if(_thrusters.begin(),_thrusters.end(),[t](const std::unique_ptr<thruster>& elem){return (*elem) == t;});
	if(it!=_thrusters.end()){
		auto copy = **it;
		_thrusters.erase(it);
		return copy;
	}
	else return thruster();
}

void thrustSystem::addContainer(fuelContainer fc)
{
	double kgs = fc.getContent();
	fc.request(kgs);
	_containers[fc.getFuelType()->ID].containers.push_back(std::make_unique<fuelContainer>(fc));
	addFuel(fc.getFuelType(),kgs);
	if(_containers[fc.getFuelType()->ID].currentContainerIndex == -1)
		_containers[fc.getFuelType()->ID].currentContainerIndex = 0;
}

fuelContainer thrustSystem::removeContainer(const fuelContainer &fc)
{
	fuelTypeContainers& fclist = _containers[fc.getFuelType()->ID];
	auto it = std::find_if(fclist.containers.begin(),fclist.containers.end(),[fc](const std::unique_ptr<fuelContainer>& elem){return (*elem) == fc;});
	if(it!=fclist.containers.end()){
		auto copy = **it;
		fclist.containers.erase(it);
		if(fclist.containers.size()!=0){
			_containers[fc.getFuelType()->ID].currentContainerIndex=0;
			getFuel(fc.getFuelType(),1-addFuel(fc.getFuelType(),1));
		}
		else{
			_containers[fc.getFuelType()->ID].currentContainerIndex=-1;
		}

		return copy;
	}
	else return fuelContainer();
}

double thrustSystem::addFuel(const fuel *f, double kg)
{
	auto &fclist = _containers[f->ID];
	if(fclist.currentContainerIndex!=-1)
	{
		while(kg > 0 && !fclist.containers[fclist.containers.size()-1]->isFull()){
			kg = fclist.containers[fclist.currentContainerIndex]->fill(kg);
			if(fclist.currentContainerIndex<fclist.containers.size()-1)
				fclist.currentContainerIndex++;
		}
	}
	return kg;
}

double thrustSystem::getFuel(const fuel *f, double kg)
{
	auto &fclist = _containers[f->ID];
	if(fclist.currentContainerIndex!=-1)
	{
		while(kg > 0 && !fclist.containers[0]->isEmpty()){
			kg = fclist.containers[fclist.currentContainerIndex]->request(kg);
			if(fclist.currentContainerIndex>0)
				fclist.currentContainerIndex--;
		}
	}
	return kg;
}

std::tuple<point3Dd, point3Dd> thrustSystem::getThrust(double dt)
{
	//TODO rigidbody calculation
	return std::make_tuple(point3Dd(),point3Dd());
}
