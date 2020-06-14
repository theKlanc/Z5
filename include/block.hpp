#pragma once
#include "graphicsManager.hpp"
#include "json.hpp"
#include "HI2.hpp"
#include "colliderManager.hpp"

using nlohmann::json;

enum class blockRotation
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

	colliderType collider = colliderType::CUBE;

	bool operator==(const baseBlock& right);
	static std::vector<baseBlock> terrainTable;

	static void loadTerrainTable();
};

enum class AO_TYPE{
	NONE,
	SINGLE_R,
	SINGLE_U,
	SINGLE_L,
	SINGLE_D,
	DOUBLE_O_H,
	DOUBLE_O_V,
	DOUBLE_A_UR,
	DOUBLE_A_LU,
	DOUBLE_A_DL,
	DOUBLE_A_RD,
	TRIPLE_R,
	TRIPLE_U,
	TRIPLE_L,
	TRIPLE_D,
	QUAD,
};

struct metaBlock
{
	baseBlock* base;
	blockRotation rotation = blockRotation::UP;
	bool flip = false;
	bool saveMeta = false;

	bool _render_visible = true; // is it (not) occluded by other blocks?
	AO_TYPE _AO = AO_TYPE::NONE;

	bool operator==(const metaBlock& right) const;

	bool operator!=(const metaBlock& right) const
	{
		return !(*this == right);
	}
	static metaBlock nullBlock;

	reactphysics3d::Quaternion getRotationQuat() const;

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

