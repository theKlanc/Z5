#include "components/projectile.hpp"

void to_json(nlohmann::json &j, const projectile &n){
	j=nlohmann::json{{"damage",n._damage}};
}

void from_json(const nlohmann::json &j, projectile &n){
	n._damage = j.at("damage").get<unsigned>();
}
