#include "block.hpp"
#include "json.hpp"
using nlohmann::json;

void to_json(nlohmann::json &j, const block &b)
{
	j = json{{"name",b.name},{"visible",b.visible},{"solid",b.solid},{"opaque",b.opaque},{"mass",b.mass}};
}

void from_json(const nlohmann::json &j, block &b)
{
	j.at("name").get_to(b.name);
	j.at("visible").get_to(b.visible);
	j.at("solid").get_to(b.solid);
	j.at("opaque").get_to(b.opaque);
	j.at("mass").get_to(b.mass);
}
