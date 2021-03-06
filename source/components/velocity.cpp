#include "components/velocity.hpp"
#include "json.hpp"
#include "fdd.hpp"

void to_json(nlohmann::json& j, const velocity& n){
	j=nlohmann::json{{"spd",n.spd}};
}

void from_json(const nlohmann::json& j, velocity& n)
{
	n.spd=j.at("spd");
}