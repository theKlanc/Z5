#include "interactables/blockSwitch.hpp"
#include "universeNode.hpp"
#include "jsonTools.hpp"

blockSwitch::blockSwitch()
{

}

void blockSwitch::interact(entt::entity e)
{
	std::vector<std::pair<metaBlock,point3Di>> newBlocks;
	for(std::pair<metaBlock,point3Di> &m : _blocks){
		newBlocks.push_back(std::make_pair<>(_parent->getBlock(m.second),m.second));
		_parent->setBlock(m.first,m.second);
	}
	_blocks=newBlocks;
}

nlohmann::json blockSwitch::getJson() const
{
	nlohmann::json blocksJJ;
	for(const std::pair<metaBlock,point3Di> &m : _blocks){
		blocksJJ.push_back(nlohmann::json{{"block",m.first},{"position",m.second}});
	}
	return nlohmann::json{{"type",BLOCK_SWITCH},{"interactable",{
	{"positions",_positions},{"blocks",blocksJJ}
			}}};
}

void from_json(const nlohmann::json &j, blockSwitch &bs)
{
	bs._blocks.clear();
	bs._positions = j.at("positions").get<std::vector<fdd>>();
	for(nlohmann::json jj : j.at("blocks")){
		bs._blocks.push_back(std::make_pair<>(jj.at("block").get<metaBlock>(),jj.at("position").get<point3Di>()));
	}
}
