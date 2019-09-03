#include "components/name.hpp"

void to_json(nlohmann::json& j, const name& n){
	j=nlohmann::json{{"nameString",n.nameString}};
}
void from_json(const nlohmann::json& j, name& n)
{
	n.nameString=j.at("nameString");
}