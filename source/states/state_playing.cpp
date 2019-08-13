#include "states/state_playing.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "gameCore.hpp"
#include "components/position.hpp"
#include "components/drawable.hpp"
#include <filesystem>
#include <variant>
#include "components/velocity.hpp"

State::Playing::Playing() {}

State::Playing::Playing(gameCore& gc, std::string saveName = "default") :State_Base(gc) {
	//create savefile folder in case it doesn't exist, and copy needed files
	if (!std::filesystem::exists(HI2::getSavesPath().string().c_str() + saveName)) {
		std::filesystem::create_directories(HI2::getSavesPath().string().c_str() + saveName);
		std::filesystem::copy_file(HI2::getDataPath().string() + "defData/universe.json", HI2::getSavesPath().string() + saveName + "/universe.json");
	}

	//load universe.json
	std::ifstream universeFile(HI2::getSavesPath().string() + saveName + "/universe.json");
	json j;
	universeFile >> j;
	j.get_to(_universeBase);
	_universeBase.linkChildren();

	//load terrain table
	loadTerrainTable();

	universeNode* result;
	_universeBase.findNodeByID(4, result);

	_player = _enttRegistry.create();
	_camera = _enttRegistry.create();

	auto& playerPos = _enttRegistry.assign<position>(_player);
	playerPos.parent = result;
	playerPos.pos.x = 10;
	playerPos.pos.y = 10;
	playerPos.pos.z = 10;
	playerPos.pos.r = 0;


	auto& cameraPos = _enttRegistry.assign<position>(_camera);
	cameraPos.parent = result;
	cameraPos.pos.x = 11;
	cameraPos.pos.y = 11;
	cameraPos.pos.z = 11;
	cameraPos.pos.r = 0;

	auto& playerSpd = _enttRegistry.assign<velocity>(_player);
	playerSpd.parent = result;
	playerSpd.spd.x = 1;
	playerSpd.spd.y = 1;
	playerSpd.spd.z = 0;
	playerSpd.spd.r = 0.1;

	auto& playerSprite = _enttRegistry.assign<drawable>(_player);

	auto& pos = _enttRegistry.get<position>(_player);

	playerSprite.sprite = _core->getGraphics().loadTexture("player");
}

void State::Playing::input() {}

void State::Playing::update(float dt) {
	position& pos = _enttRegistry.get<position>(_player);
	_universeBase.updateChunks(pos.pos, pos.parent);

	auto movableEntityView = _enttRegistry.view<velocity, position>();
	for (const entt::entity& entity : movableEntityView) { // afegim les entitats dibuixables
		velocity vel = movableEntityView.get<velocity>(entity);
		position& pos = movableEntityView.get<position>(entity);

		pos.pos += (vel.spd * dt);
	}
}

void State::Playing::draw() {

	std::vector<renderLayer> renderOrders;
	HI2::setBackgroundColor(RGBA8(0, 0, 0, 255));
	{
		position cameraPos = _enttRegistry.get<position>(_camera);
		std::vector<universeNode*> sortedDrawingNodes = _universeBase.nodesToDraw(cameraPos.pos, cameraPos.parent);


		for (int i = config::cameraDepth; i >= 0; --i) {//for depth afegim cada capa dels DrawingNodes
			position currentCameraPos = cameraPos;
			currentCameraPos.pos.z -= i;
			for (universeNode*& node : sortedDrawingNodes) {
				//obtenir posicio de la camera al node
				fdd localCameraPos = node->getLocalPos(currentCameraPos.pos, currentCameraPos.parent);
				//obtenir profunditat
				int layer = floor(localCameraPos.z);

				double partFraccional = fmod(localCameraPos.z, 1);
				double depth = i - partFraccional - 1;

				renderOrders.push_back(renderLayer{ depth,std::variant<entt::entity,nodeLayer>(nodeLayer{node,layer}) });
			}
		}

		auto drawableEntityView = _enttRegistry.view<drawable, position>();
		for (auto entity : drawableEntityView) { // afegim les entitats dibuixables
			auto& pos = drawableEntityView.get<position>(entity);
			double depth = cameraPos.pos.z - cameraPos.parent->getLocalPos(pos.pos, pos.parent).z;
			renderOrders.push_back(renderLayer{ depth,	std::variant<entt::entity,nodeLayer>(entity) });
		}

	}
	//ordenem per profunditat
	std::sort(renderOrders.begin(), renderOrders.end(), [](renderLayer& l, renderLayer& r) {
		return l.depth > r.depth;
	});

	HI2::startFrame();
	for (renderLayer& rl : renderOrders) {
		drawLayer(rl);
	}
	HI2::endFrame();

}

void State::Playing::drawLayer(const State::Playing::renderLayer& rl)
{
	struct visitor {
		void operator()(const entt::entity& entity) const {
			const drawable& sprite = registry->get<drawable>(entity);
			const position& entityPosition = registry->get<position>(entity);
			fdd localPos = cameraPos.parent->getLocalPos(entityPosition.pos, entityPosition.parent) - cameraPos.pos;
			
			localPos.x *= config::spriteSize;
			localPos.y *= config::spriteSize;
			
			localPos.x += HI2::getScreenWidth() / 2;
			localPos.y += HI2::getScreenHeight() / 2;

			localPos.x -= config::spriteSize/2;
			localPos.y -= config::spriteSize/2;
			// falta fer scaling segons depth
			HI2::drawTexture(*sprite.sprite, localPos.x, localPos.y, 1, localPos.r);
		}
		void operator()(const nodeLayer& node) const {
			// here we should draw a nodeLayer
		}
		entt::registry* registry;
		position cameraPos;
	};
	visitor v;
	v.registry = &_enttRegistry;
	v.cameraPos = _enttRegistry.get<position>(_camera);
	std::visit(v, rl.target);
}

void State::Playing::loadTerrainTable()
{
	std::ifstream terrainTableFile(HI2::getDataPath().string() + "blockTable.json");
	json j;
	terrainTableFile >> j;
	j.get_to(_terrainTable);
	for (block& b : _terrainTable) {
		if (b.visible) {
			b.texture = _core->getGraphics().loadTexture(b.name);
		}
	}
	block::terrainTable = _terrainTable;
}

