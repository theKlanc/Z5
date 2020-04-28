#include "thrustSystem.hpp"

thrustSystem::thrustSystem()
{

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
