#include "states/state_playing.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "gameCore.hpp"
#include "components/position.hpp"
#include "components/drawable.hpp"
#include <filesystem>
#include <variant>
#include <components\test.hpp>

State::Playing::Playing() {}

State::Playing::Playing(gameCore &gc, std::string saveName = "default"):State_Base(gc){
	//create savefile folder in case it doesn't exist, and copy needed files
	if(!std::filesystem::exists(HI2::getSavesPath().string().c_str() + saveName)){
		std::filesystem::create_directories(HI2::getSavesPath().string().c_str() + saveName);
		std::filesystem::copy_file(HI2::getDataPath().string()+"defData/universe.json",HI2::getSavesPath().string() + saveName + "/universe.json");
	}

	//load universe.json
	std::ifstream universeFile(HI2::getSavesPath().string() + saveName + "/universe.json");
	json j;
	universeFile >> j;
	j.get_to(_universeBase);
	_universeBase.linkChildren();

	//load terrain table
	loadTerrainTable();

	_player = _enttRegistry.create();
	_camera = _enttRegistry.create();

	auto& playerPos = _enttRegistry.assign<position>(_player);
	auto& cameraPos = _enttRegistry.assign<position>(_camera);
	auto& playerSprite = _enttRegistry.assign<drawable>(_player);
	auto& b = _enttRegistry.assign<test>(_player);
	
	universeNode* result;
	_universeBase.findNodeByID(4,result);

	
	playerPos.parent=result;
	playerPos.pos.x=10;
	playerPos.pos.y=10;
	playerPos.pos.z=10;
	playerPos.pos.r=0;

	auto& pos =_enttRegistry.get<position>(_player);
	
	

	cameraPos.parent=result;
	cameraPos.pos.x=10;
	cameraPos.pos.y=10;
	cameraPos.pos.z=10;
	cameraPos.pos.r=0;
	
	playerSprite.sprite=_core->getGraphics().loadTexture("player");

	b.t=true;
	
	
	auto templmao = _enttRegistry.get<test>(_player);
}

void State::Playing::input() {}

void State::Playing::update(float dt) {
	position& pos = _enttRegistry.get<position>(_player);
	bool templmao =false;
	templmao= _enttRegistry.get<test>(_player).t;
	_universeBase.updateChunks(pos.pos,pos.parent);
}

void State::Playing::draw() {

	std::vector<renderLayer> renderOrders;

	{
		position cameraPos = _enttRegistry.get<position>(_camera);
		std::vector<universeNode*> sortedDrawingNodes = _universeBase.nodesToDraw(cameraPos.pos,cameraPos.parent);


		for(int i = config::cameraDepth; i >=0;--i){//for depth afegim cada capa dels DrawingNodes
			position currentCameraPos = cameraPos;
			currentCameraPos.pos.z-=i;
			for(universeNode*& node : sortedDrawingNodes){
				//obtenir posicio de la camera al node
				fdd localCameraPos = node->getLocalPos(currentCameraPos.pos,currentCameraPos.parent);
				//obtenir profunditat
				int layer = floor(localCameraPos.z);

				double trash = 0;
				double partFraccional= fmod(localCameraPos.z,trash);
				double depth=i-partFraccional-1;

				renderOrders.push_back(renderLayer{depth,std::variant<entt::entity,nodeLayer>(nodeLayer{node,layer})});
			}
		}

		auto drawableEntityView = _enttRegistry.view<drawable,position>();
		for(auto entity : drawableEntityView){ // afegim les entitats dibuixables
			auto &pos = drawableEntityView.get<position>(entity);
			double depth = cameraPos.pos.z - cameraPos.parent->getLocalPos(pos.pos,pos.parent).z;
			renderOrders.push_back(renderLayer{depth,	std::variant<entt::entity,nodeLayer>(entity)});
		}

	}
	//ordenem per profunditat
	std::sort(renderOrders.begin(),renderOrders.end(),[](renderLayer& l, renderLayer& r){
		return l.depth>r.depth;
	});

	HI2::startFrame();
	for(renderLayer& rl : renderOrders){
		drawLayer(rl);
	}
	HI2::endFrame();

}

void State::Playing::drawLayer(const State::Playing::renderLayer &rl)
{
	struct visitor{
		void operator()(const entt::entity& entity) const{
			const drawable& sprite = registry.get<drawable>(entity);
			HI2::drawTexture(*sprite.sprite,0,0,0);
		}
		void operator()(const nodeLayer& node) const{
			// here we should draw a nodeLayer
		}
		entt::DefaultRegistry registry;
	};
	visitor v;
	std::visit(v,rl.target);
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

