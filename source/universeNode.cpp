#include "universeNode.hpp"
#include "fdd.hpp"
#include "config.hpp"

universeNode::universeNode()
{

}

block& universeNode::getBlock(const point3Di& pos)
{
	return chunkAt(pos).getBlock(pos);
}

void universeNode::setBlock(block* b, const point3Di &pos)
{
	chunkAt(pos).setBlock(b,pos);
}

terrainChunk &universeNode::chunkAt(const point3Di &pos)
{
	return _chunks[(pos.x/config::chunkSize%config::chunkLoadRadius*config::chunkLoadRadius*config::chunkLoadRadius)
			+(pos.y/config::chunkSize%config::chunkLoadRadius*config::chunkLoadRadius)
			+(pos.z/config::chunkSize%config::chunkLoadRadius)];
}

void universeNode::linkChildren()
{
	for(universeNode& node : _children){
		node._parent=this;
	}
}

void to_json(nlohmann::json &j, const universeNode &f)
{
	j = json{{"name",f._name},{"mass",f._mass},{"diameter",f._diameter},{"type",f._type},{"position",f._position},{"velocity",f._velocity},{"children",f._children}};
}

void from_json(const json& j, universeNode& f){
	f._parent=nullptr;
	f._name=j.at("name").get<std::string>();
	f._type=j.at("type").get<nodeType>();
	f._mass=j.at("mass").get<double>();
	f._diameter=j.at("diameter").get<double>();
	f._position=j.at("position").get<fdd>();
	f._velocity=j.at("velocity").get<fdd>();
	f._children = std::vector<universeNode>();
	for (const nlohmann::json& element : j.at("children")) {
		f._children.push_back(element.get<universeNode>());
	}
	f.linkChildren();

}
