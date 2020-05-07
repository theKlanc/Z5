#include "interactables/blockSwitch.hpp"
#include "universeNode.hpp"
#include "jsonTools.hpp"
#include "icecream.hpp"

blockSwitch::blockSwitch()
{

}

void blockSwitch::interact(entt::entity e)
{
	IC(_sfx);
	if(_sfx != nullptr){
		HI2::playSound(*_sfx);
		IC();
	}
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
	auto result = nlohmann::json{{"type",interactableType::BLOCK_SWITCH},{"interactable",{{"positions",_positions},{"blocks",blocksJJ}}}};
	if(_sfx != nullptr)
		result.at("interactable").emplace("sfx",_sfxname);
	return result;
}

void from_json(const nlohmann::json &j, blockSwitch &bs)
{
	bs._blocks.clear();
	bs._positions = j.at("positions").get<std::vector<fdd>>();
	if(j.contains("sfx"))
	{
		IC("contains sfx yaay");
		bs._sfxname = j.at("sfx").get<std::string>();
		bs._sfx = Services::audio.loadAudio(bs._sfxname);
		IC(bs._sfx);
	}
	for(nlohmann::json jj : j.at("blocks")){
		bs._blocks.push_back(std::make_pair<>(jj.at("block").get<metaBlock>(),jj.at("position").get<point3Di>()));
	}
}
