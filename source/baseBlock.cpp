#include "block.hpp"
#include "json.hpp"
using nlohmann::json;

bool baseBlock::operator==(const baseBlock& right)
{
	return ID==right.ID;
}

void to_json(nlohmann::json &j, const baseBlock &b)
{
	j = json{{"name",b.name},{"ID",b.ID},{"visible",b.visible},{"solid",b.solid},{"opaque",b.opaque},{"mass",b.mass}};
}

void from_json(const nlohmann::json &j, baseBlock &b)
{
	j.at("name").get_to(b.name);
	j.at("ID").get_to(b.ID);
	j.at("visible").get_to(b.visible);
	j.at("solid").get_to(b.solid);
	j.at("opaque").get_to(b.opaque);
	j.at("mass").get_to(b.mass);
}

std::vector<baseBlock> baseBlock::terrainTable;