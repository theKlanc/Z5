#include "fuelContainer.hpp"

bool fuelContainer::isEmpty() const
{
	return _content == 0;
}

bool fuelContainer::isFull() const
{
	return _content == _capacity;
}

void from_json(const nlohmann::json &j, fuelContainer &c)
{
	unsigned fueltofind = j.at("fuelID").get<unsigned>();
	c._fuelType = std::find_if(fuel::fuelList.begin(),fuel::fuelList.end(),[fueltofind](const fuel& f) { return f.ID == fueltofind;}).base();
	c._content = j.at("content").get<double>();
	c._capacity = j.at("capacity").get<double>();
}

void to_json(nlohmann::json &j, const fuelContainer &c)
{
	j = nlohmann::json{{"fuelID",c._fuelType->ID},{"capacity",c._capacity},{"content",c._content}};
}
