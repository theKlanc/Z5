#include "fuelContainer.hpp"
#include "jsonTools.hpp"

double fuelContainer::fill(double mass)
{
	double freeSpace = _capacity-_content;
	if(freeSpace > mass){
		_content+=mass;
		return 0;
	}
	else{
		_content=_capacity;
		return mass - freeSpace;
	}
}

double fuelContainer::request(double mass)
{
	if(_content > mass){
		_content-=mass;
		return 0;
	}
	else{
		double contentTemp = _content;
		_content=0;
		return mass - contentTemp;
	}
}

double fuelContainer::getContent()
{
	return _content;
}

bool fuelContainer::isEmpty() const
{
	return _content == 0;
}

bool fuelContainer::isFull() const
{
	return _content == _capacity;
}

const fuel *fuelContainer::getFuelType() const
{
	return _fuelType;
}

bool fuelContainer::operator==(const fuelContainer &fc) const
{
	return _pos == fc._pos; //According to the Pauli exclusion principle, two containers cannot coexist in the same exact position, therefore we only need to check for equal positions when comparing
}

void from_json(const nlohmann::json &j, fuelContainer &c)
{
	unsigned fueltofind = j.at("fuelID").get<unsigned>();
	c._fuelType = std::find_if(fuel::fuelList.begin(),fuel::fuelList.end(),[fueltofind](const fuel& f) { return f.ID == fueltofind;}).base();
	c._content = j.at("content").get<double>();
	c._capacity = j.at("capacity").get<double>();
	c._pos = j.at("position").get<point3Di>();
}

void to_json(nlohmann::json &j, const fuelContainer &c)
{
	j = nlohmann::json{{"fuelID",c._fuelType->ID},{"capacity",c._capacity},{"content",c._content},{"position",c._pos}};
}
