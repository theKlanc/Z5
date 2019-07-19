#pragma once
#include "HardwareInterface/HardwareInterface.hpp"

struct block{ // A block represents a 1m³ cube of material
	bool visible = true; // can it be rendered?
	bool solid = true; // can an entity be inside it? Entities will float on non-solids according to their mass/buoyancy
	bool opaque = true; // can we avoid rendering blocks under it?
	long mass = 1220000; // mass in grams

	HI2::Texture* texture = nullptr;
};
