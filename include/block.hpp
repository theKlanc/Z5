#pragma once
#include "graphicsManager.hpp"
#include "json.hpp"
#include "HardwareInterface/HardwareInterface.hpp"

using nlohmann::json;

enum blockRotation
{
	UP = 0,
	LEFT = 1,
	DOWN = 2,
	RIGHT = 3,
};

blockRotation operator++(blockRotation& a, int);
blockRotation operator--(blockRotation& a, int);

struct baseBlock { // A baseBlock represents a 1m³ cube of material
	std::string name;
	unsigned ID;
	bool visible = false; // can it be rendered?
	bool solid = false; // can an entity be inside it? Entities will float on non-solids according to their mass/buoyancy
	bool opaque = false; // can we avoid rendering blocks under it?
	double mass = 1220; // mass in kg
	sprite* spr = nullptr;

	bool operator==(const baseBlock& right);
	static std::vector<baseBlock> terrainTable;
};

struct metaBlock
{
	baseBlock* base;
	blockRotation rotation = UP;
	bool saveMeta = false;
	bool operator==(const metaBlock& right);

	bool operator!=(const metaBlock& right)
	{
		return !(*this == right);
	}
	static metaBlock nullBlock;

	friend std::ostream& operator<<(std::ostream& os, const metaBlock& m);
	friend std::istream& operator>>(std::istream& is, const metaBlock& m);
	
};

std::ostream& operator<<(std::ostream& os, const metaBlock& m);
std::istream& operator>>(std::istream& is, const metaBlock& m);

void to_json(json& j, const baseBlock& b);
void from_json(const json& j, baseBlock& b);