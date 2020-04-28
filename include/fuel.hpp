#pragma once
#include <string>
#include <vector>
#include "json.hpp"

struct fuel{
	unsigned ID = 0;
	std::string name = "water";
	double density = 1000; //In kg/m³ at normal storage pressure and temp, doesn't matter at all
	double specificEnergy = 0; //In MJ/kg, shouldnt matter either

	static std::vector<fuel> fuelList;
	static void loadFuelList();
};

void to_json(nlohmann::json& j, const fuel& f);
void from_json(const nlohmann::json& j, fuel& f);
