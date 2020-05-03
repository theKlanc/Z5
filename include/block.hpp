#pragma once
#include "graphicsManager.hpp"
#include "json.hpp"
#include "HI2.hpp"

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

	static void loadTerrainTable();
};

struct metaBlock
{
	baseBlock* base;
	blockRotation rotation = UP;
	bool flip = false;
	bool saveMeta = false;
	bool operator==(const metaBlock& right) const;

	bool operator!=(const metaBlock& right) const
	{
		return !(*this == right);
	}
	static metaBlock nullBlock;

	friend std::ostream& operator<<(std::ostream& os, const metaBlock& m);
	//friend std::istream& operator>>(std::istream& is, const metaBlock& m);
	
};

std::ostream& operator<<(std::ostream& os, const metaBlock& m);
//std::istream& operator>>(std::istream& is, metaBlock& m);
std::ostream& operator<<(std::ostream& os, const std::vector<metaBlock>& m);
std::istream& operator>>(std::istream& is, std::vector<metaBlock>& m);

void to_json(json& j, const baseBlock& b);
void from_json(const json& j, baseBlock& b);

void to_json(json& j, const metaBlock& b);
void from_json(const json& j, metaBlock& b);

