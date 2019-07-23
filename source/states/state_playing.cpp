#include "states/state_playing.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "gameCore.hpp"

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
	_enttRegistry.create();
}

void State::Playing::input() {}

void State::Playing::update(float dt) {}

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
