#include <iostream>
#include "services.hpp"
#include "block.hpp"
#include "json.hpp"

using nlohmann::json;

blockRotation operator++(blockRotation& a, int)
{
	switch (a)
	{
	case UP:
		a = LEFT;
		break;
	case LEFT:
		a = DOWN;
		break;
	case DOWN:
		a = RIGHT;
		break;
	case RIGHT:
		a = UP;
		break;
	}
	return a;
}

blockRotation operator--(blockRotation& a, int)
{
	switch (a)
	{
	case DOWN:
		a = LEFT;
		break;
	case RIGHT:
		a = DOWN;
		break;
	case UP:
		a = RIGHT;
		break;
	case LEFT:
		a = UP;
		break;
	}
	return a;
}

bool baseBlock::operator==(const baseBlock& right)
{
	return ID == right.ID;
}

void baseBlock::loadTerrainTable()
{
	terrainTable.clear();
	std::ifstream terrainTableFile(HI2::getDataPath().append("blockTable.json"));
	json j;
	terrainTableFile >> j;
	j.get_to(baseBlock::terrainTable);
	metaBlock::nullBlock.base = &baseBlock::terrainTable[0];
}

bool metaBlock::operator==(const metaBlock& right)
{
	if (saveMeta == right.saveMeta && saveMeta == false)
		return base == right.base;
	return base == right.base && rotation == right.rotation;
}

std::ostream& operator<<(std::ostream& os, const metaBlock& m)
{
	if (m.saveMeta)
	{
		return os << m.base->ID << ' ' << m.saveMeta << ' ' << m.rotation;
	}
	else {
		return os << m.base->ID << ' ' << m.saveMeta;
	}
}

void to_json(nlohmann::json& j, const baseBlock& b)
{
	j = json{ {"name",b.name},{"ID",b.ID},{"visible",b.visible},{"solid",b.solid},{"opaque",b.opaque},{"mass",b.mass} };
	if (b.spr != nullptr) {
		j.push_back({ "frames",b.spr->getAllFrames() });
	}
}

void from_json(const nlohmann::json& j, baseBlock& b)
{
	j.at("name").get_to(b.name);
	j.at("ID").get_to(b.ID);
	j.at("visible").get_to(b.visible);
	j.at("solid").get_to(b.solid);
	j.at("opaque").get_to(b.opaque);
	j.at("mass").get_to(b.mass);
	std::vector<frame> frames;
	if (j.contains("sprite")) {
		for (const nlohmann::json& element : j.at("sprite").at("frames")) {
			frames.push_back(element.get<frame>());
		}
		b.spr = Services::graphics.loadSprite(b.name, "spritesheet", frames);
	}
}

std::vector<baseBlock> baseBlock::terrainTable;
metaBlock metaBlock::nullBlock;