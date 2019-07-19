#include "universeNode.hpp"
#include "fdd.hpp"
#include "jsonFactory.hpp"

universeNode::universeNode(const nlohmann::json &j, universeNode* parent):_type(j.at("type").get<nodeType>())
{
	_name=j.at("name").get<std::string>();
	_mass=j.at("mass").get<double>();
	_diameter=j.at("diameter").get<double>();
	_position=j.at("position").get<fdd>();
	_velocity=j.at("velocity").get<fdd>();
	_parent=parent;
	_children = std::vector<universeNode>();
	for (const nlohmann::json& element : j.at("children")) {
		_children.push_back(jsonFactory::getUniverseNode(element));
	}
}

void to_json(nlohmann::json &j, const universeNode &f)
{
	j = json{{"name",f._name},{"mass",f._mass},{"diameter",f._diameter},{"type",f._type},{"position",f._position},{"velocity",f._velocity},{"children",f._children}};
}