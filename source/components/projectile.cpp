#include "components/projectile.hpp"

void to_json(nlohmann::json &j, const projectile &n){
	j=nlohmann::json{{"damage",n._damage},{"remainingPenetration",n._remainingPenetration},{"remainingBounces",n._remainingBounces}};
}

void from_json(const nlohmann::json &j, projectile &n){
	n._damage = j.at("damage").get<unsigned>();
	n._remainingBounces = j.at("remainingBounces").get<int>();
	n._remainingPenetration = j.at("_remainingPenetration").get<int>();
}
