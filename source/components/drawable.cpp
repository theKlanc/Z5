#include "components/drawable.hpp"
#include "json.hpp"

void to_json(nlohmann::json& j, const drawable& n){
	j=nlohmann::json{{"spriteName",n.name}};
}
void from_json(const nlohmann::json& j, drawable& n)
{
	n.name=j.at("spriteName");
}