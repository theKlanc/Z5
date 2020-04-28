#include "fuel.hpp"
#include <fstream>
#include "HardwareInterface/HI2.hpp"
#include <vector>

std::vector<fuel> fuel::fuelList;

void fuel::loadFuelList()
{
	fuelList.clear();
	std::ifstream fuelListFile(HI2::getDataPath().append("fuelList.json"));
	nlohmann::json j;
	fuelListFile >> j;
	j.get_to(fuelList);
}

void to_json(nlohmann::json &j, const fuel &f)
{
	j = nlohmann::json{{"ID",f.ID},{"name",f.name},{"density",f.density},{"energy",f.specificEnergy}};
}

void from_json(const nlohmann::json &j, fuel &f)
{
	f.ID = j.at("ID").get<unsigned>();
	f.name = j.at("name").get<std::string>();
	f.density = j.at("density").get<double>();
	f.specificEnergy = j.at("energy").get<double>();
}
