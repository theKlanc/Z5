#include "thrustSystem.hpp"
#include "fuel.hpp"
#include <algorithm>
#include "jsonTools.hpp"
#include "icecream.hpp"

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

void thrustSystem::update(double dt)
{
	for(auto& t : _thrusters){
		if(t)
			t->update(dt);
	}
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
	std::unordered_map<unsigned,double> massRequired;

	for(auto ft : fuel::fuelList){
		massRequired.emplace(ft.ID,0);
	}

	for(auto &t : _thrusters){
		massRequired[t->getFuelType()->ID]+=t->getConsumption() * dt;
	}

	std::unordered_map<unsigned,double> fuelPercent;
	for(auto ft : fuel::fuelList){
		double percent = (massRequired[ft.ID] - getFuel(&ft,massRequired[ft.ID]))/massRequired[ft.ID];
		if(!std::isnormal(percent))
			percent = 0;
		fuelPercent.emplace(ft.ID,percent);
	}

	//TODO real rigidbody calculation
	point3Dd totalThrust;
	for(auto &t : _thrusters){
		auto [thrust, position] = t->getThrustVector(t->getConsumption()*dt*fuelPercent[t->getFuelType()->ID],dt);
		totalThrust+=thrust;

	}

	assert(!std::isnan(totalThrust.x));
	return std::make_tuple(totalThrust,point3Dd());
}

void thrustSystem::setThrustTarget(point3Dd target)
{
	for(auto& thruster : _thrusters){
		auto direction = thruster->getThrustDirection();
		//X
		if(direction.x == 1){
			thruster->setTargetThrust(target.x>=0?target.x:0);
		}
		if(direction.x == -1){
			thruster->setTargetThrust(target.x<=0?-target.x:0);
		}

		//Y
		if(direction.y == 1){
			thruster->setTargetThrust(target.y>=0?target.y:0);
		}
		if(direction.y == -1){
			thruster->setTargetThrust(target.y<=0?-target.y:0);
		}

		//Z
		if(direction.z == 1){
			thruster->setTargetThrust(target.z>=0?target.z:0);
		}
		if(direction.z == -1){
			thruster->setTargetThrust(target.z<=0?-target.z:0);
		}
	}
}


void to_json(nlohmann::json &j, const fuelTypeContainers &t){
	auto list = nlohmann::json{};
	for(auto &c : t.containers){
		list.push_back(*c);
	}
	j = nlohmann::json{{"containers",list},{"index",t.currentContainerIndex}};
}

void from_json(const nlohmann::json &j, fuelTypeContainers &t)
{
	t.currentContainerIndex = j.at("index").get<unsigned>();
	for(auto jj : j.at("containers")){
		auto temp = std::make_unique<fuelContainer>();
		from_json(jj,*temp);
		t.containers.push_back(std::move(temp));
	}
}

void to_json(nlohmann::json &j, const thrustSystem &t)
{
	auto containerList = nlohmann::json{};
	for(auto &c : t._containers){
		containerList.push_back({{"fuelID",c.first},{"ftContainers",c.second}});
	}

	auto thrusterList = nlohmann::json{};
	for(auto &c : t._thrusters){
		thrusterList.push_back(*c);
	}

	j = nlohmann::json{{"containers",containerList},{"thrusters",thrusterList}};
}

void from_json(const nlohmann::json &j, thrustSystem &t)
{
	t._thrusters.clear();
	t._containers.clear();
	for(auto jj : j.at("containers")){
		t._containers.emplace(jj.at("fuelID").get<unsigned>(),jj.at("ftContainers").get<fuelTypeContainers>());
	}
	for(auto jj : j.at("thrusters")){
		t._thrusters.push_back(std::make_unique<thruster>(jj.get<thruster>()));
	}
}
