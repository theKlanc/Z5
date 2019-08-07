#include "states/state_playing.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "gameCore.hpp"
#include "components/position.hpp"
#include "components/drawable.hpp"
#include <variant>

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
	playerSprite.sprite=_core->getGraphics().loadTexture("player");
}

void State::Playing::input() {}

void State::Playing::update(float dt) {
	position& pos = _enttRegistry.get<position>(_player);
	_universeBase.updateChunks(pos.pos,pos.parent);
}

void State::Playing::draw() {

	std::vector<renderLayer> renderOrders;


	{
		position cameraPos = _enttRegistry.get<position>(camera);
		std::vector<universeNode*> sortedDrawingNodes = _universeBase.nodesToDraw(cameraPos.pos,cameraPos.parent);
		std::sort(sortedDrawingNodes.begin(),sortedDrawingNodes.end(),[](universeNode* a, universeNode* b) {
			return a->drawBefore(*b);
		});
		//for depth afegim cada capa dels DrawingNodes
		//for drawableEntity insertem a renderOrders de manera ordenada
	}
	HI2::startFrame();
	for(renderLayer& rl : renderOrders){
		drawLayer(rl);
	}
	HI2::endFrame();

}

void State::Playing::drawLayer(const State::Playing::renderLayer &rl)
{
	struct {
		void operator()(const entt::entity& entity) const{
			// here we should draw an entity
		}
		void operator()(const universeNode* node) const{
			// here we should draw a node slice
		}
	} visitor;
	std::visit(visitor,rl.target);
}

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
