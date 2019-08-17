#pragma once
#include "json.hpp"
#include "HardwareInterface/HardwareInterface.hpp"

using nlohmann::json;

struct block{ // A block represents a 1m³ cube of material
	std::string name;
	unsigned ID;
	bool visible = true; // can it be rendered?
	bool solid = true; // can an entity be inside it? Entities will float on non-solids according to their mass/buoyancy
	bool opaque = true; // can we avoid rendering blocks under it?
	double mass = 1220; // mass in kg
	HI2::Texture* texture = nullptr;

	
	static std::vector<block> terrainTable;
};

void to_json(json& j, const block& b);
void from_json(const json& j, block& b);