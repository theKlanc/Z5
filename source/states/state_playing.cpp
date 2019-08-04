#include "states/state_playing.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "gameCore.hpp"
#include "components/position.hpp"
#include "components/drawable.hpp"

State::Playing::Playing() {}

State::Playing::Playing(gameCore &gc, std::string saveName = "default"):State_Base(gc){
	//create savefile folder in case it doesn't exist, and copy needed files
	if(!std::filesystem::exists(HI2::getSavesPath().c_str() + saveName)){
		std::filesystem::create_directory(HI2::getSavesPath().c_str() + saveName);
		std::filesystem::copy_file(HI2::getDataPath().string()+"defData/universe.json",HI2::getSavesPath().string() + saveName + "/universe.json");
	}

	//load universe.json
	std::ifstream universeFile(HI2::getSavesPath().string() + saveName + "/universe.json");
	json j;
	universeFile >> j;
	j.get_to(_universeBase);

	//load terrain table
	loadTerrainTable();

	//
	_player = _enttRegistry.create();
	position& playerPos = _enttRegistry.assign<position>(_player);
	drawable& playerSprite = _enttRegistry.assign<drawable>(_player);
	universeNode* result;
	_universeBase.findNodeByID(4,result);
	playerPos.parent=result;
	playerPos.pos={10,10,10};
	playerSprite.sprite=*_core->getGraphics().loadTexture("player");
}

void State::Playing::input() {}

void State::Playing::update(float dt) {
	position& pos = _enttRegistry.get<position>(_player);
	_universeBase.updateChunks(pos.pos,pos.parent);
}

void State::Playing::draw() {}

void State::Playing::loadTerrainTable()
{
	std::ifstream terrainTableFile(HI2::getDataPath().string() + "blockTable.json");
	json j;
	terrainTableFile >> j;
	j.get_to(_terrainTable);
	for(block& b : _terrainTable){
		if(b.visible){
			b.texture = _core->getGraphics().loadTexture(b.name);
		}
	}
}
