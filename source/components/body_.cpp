#include "components/body.hpp"

void to_json(nlohmann::json& j, const body& n){
	j=nlohmann::json{{"height",n.height},{"width",n.width}};
}
void from_json(const nlohmann::json& j, body& n)
{
	n.height=j.at("height");
	n.width=j.at("width");
}