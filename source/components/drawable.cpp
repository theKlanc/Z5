#include "components/drawable.hpp"
#include "json.hpp"
#include "services.hpp"

void to_json(nlohmann::json& j, const drawable& n){
	j=nlohmann::json{{"spriteName",n.name},{"textureName",n.spr->getTextureName()},{"zoom",n.zoom},{"frames",n.spr->getAllFrames()}};
}
void from_json(const nlohmann::json& j, drawable& n)
{
	n.name=j.at("spriteName");
	if(j.contains("zoom"))
		n.zoom = j.at("zoom");
	std::vector<frame> frames;
	for (const nlohmann::json& element : j.at("frames")) {
		frames.push_back(element.get<frame>());
	}
	n.spr = Services::graphics.loadSprite(n.name,j.at("textureName"),frames);
}