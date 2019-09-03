#include "components/position.hpp"

void to_json(nlohmann::json& j, const position& n){
	j=nlohmann::json{{"pos",n.pos},{"parent",n.parentID}};
}

void from_json(const nlohmann::json& j, position& n)
{
	n.pos=j.at("pos");
	n.parentID=j.at("parent");
}