#include "states/state_playing.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include "gameCore.hpp"
#include "components/position.hpp"
#include "components/drawable.hpp"
#include <filesystem>
#include <variant>
#include "config.hpp"
#include "components/velocity.hpp"

std::filesystem::path  State::Playing::savePath;

State::Playing::Playing() {}

State::Playing::Playing(gameCore& gc, std::string saveName = "default") :State_Base(gc) {
	savePath = HI2::getSavesPath().append(saveName);
	//create savefile folder in case it doesn't exist, and copy needed files
	if (!std::filesystem::exists(savePath)) {
		std::filesystem::create_directories(savePath);
		std::cout << HI2::getDataPath().append("defData").append("universe.json") << std::endl;
		std::filesystem::copy_file(HI2::getDataPath().append("defData").append("universe.json"), savePath.append("universe.json"));
	}

	//load universe.json
	std::ifstream universeFile(savePath.append("universe.json"));
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
	entt::entity dog = _enttRegistry.create();

	auto& playerSprite = _enttRegistry.assign<drawable>(_player);
	playerSprite.sprite = _core->getGraphics().loadTexture("player");

	auto& dogSprite = _enttRegistry.assign<drawable>(dog);
	dogSprite.sprite = _core->getGraphics().loadTexture("gromit");

	auto& dogPos = _enttRegistry.assign<position>(dog);
	dogPos.parent = result;
	dogPos.pos.x = 203.5;
	dogPos.pos.y = 203.5;
	dogPos.pos.z = dogPos.parent->getHeight({ (int)dogPos.pos.x,(int)dogPos.pos.y }) + 0.5;
	dogPos.pos.r = 0;

	auto& dogSpd = _enttRegistry.assign<velocity>(dog);
	dogSpd.parent = result;
	dogSpd.spd.x = 0;
	dogSpd.spd.y = 0;
	dogSpd.spd.z = 0;
	dogSpd.spd.r = -0.1;

	auto& playerPos = _enttRegistry.assign<position>(_player);
	playerPos.parent = result;
	playerPos.pos.x = 200.5;
	playerPos.pos.y = 200.5;
	playerPos.pos.z = playerPos.parent->getHeight({ (int)playerPos.pos.x,(int)playerPos.pos.y }) + 0.5;
	playerPos.pos.r = 0;

	auto& playerSpd = _enttRegistry.assign<velocity>(_player);
	playerSpd.parent = result;
	playerSpd.spd.x = 0;
	playerSpd.spd.y = 0;
	playerSpd.spd.z = 0;
	playerSpd.spd.r = 0.1;

	auto& cameraPos = _enttRegistry.assign<position>(_camera);
	cameraPos.parent = result;
	cameraPos.pos.x = 200;
	cameraPos.pos.y = 200;
	cameraPos.pos.z = cameraPos.parent->getHeight({ (int)cameraPos.pos.x,(int)cameraPos.pos.y }) + 0.5;
	cameraPos.pos.r = 0;

	auto& cameraSpd = _enttRegistry.assign<velocity>(_camera);
	cameraSpd.parent = result;
	cameraSpd.spd.x = 0;
	cameraSpd.spd.y = 0;
	cameraSpd.spd.z = 0;
	cameraSpd.spd.r = 0;

}

void State::Playing::input(float dt)
{
	auto& cameraPos = _enttRegistry.get<position>(_camera);
	auto& cameraSpd = _enttRegistry.get<velocity>(_camera);
	int held = HI2::getKeysHeld();

	if (held & HI2::BUTTON::KEY_MINUS) {
		cameraSpd.spd = fdd();
	}
	if (held & HI2::BUTTON::KEY_UP) {
		cameraSpd.spd.y -= 3 * dt;
	}
	if (held & HI2::BUTTON::KEY_DOWN) {
		cameraSpd.spd.y += 3 * dt;
	}
	if (held & HI2::BUTTON::KEY_LEFT) {
		cameraSpd.spd.x -= 3 * dt;
	}
	if (held & HI2::BUTTON::KEY_RIGHT) {
		cameraSpd.spd.x += 3 * dt;
	}
	if (held & HI2::BUTTON::KEY_A) {
		cameraPos.pos.z += 6 * dt;
	}
	if (held & HI2::BUTTON::KEY_B) {
		cameraPos.pos.z -= 6 * dt;
	}
	if (held & HI2::BUTTON::KEY_PLUS) {
		_core->quit();
	}
}

void State::Playing::update(float dt) {
	auto movableEntityView = _enttRegistry.view<velocity, position>();
	for (const entt::entity& entity : movableEntityView) { //Update entities' positions
		velocity vel = movableEntityView.get<velocity>(entity);
		position& pos = movableEntityView.get<position>(entity);

		pos.pos += (vel.spd * dt);
	}

	position& cameraPosition = _enttRegistry.get<position>(_camera);
	_universeBase.updateChunks(cameraPosition.pos, cameraPosition.parent);//Update chunks

	std::cout << "CameraHeight: " << cameraPosition.pos.z << std::endl;
}

void State::Playing::draw() {

	std::vector<renderLayer> renderOrders;
	HI2::setBackgroundColor(HI2::Color(20, 5, 100, 255));
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
				double depth = i + partFraccional;

				renderOrders.push_back(renderLayer{ depth,std::variant<entt::entity,nodeLayer>(nodeLayer{node,layer}) });
			}
		}

		auto drawableEntityView = _enttRegistry.view<drawable, position>();
		for (auto entity : drawableEntityView) { // afegim les entitats dibuixables
			auto& pos = drawableEntityView.get<position>(entity);
			double depth = cameraPos.pos.z - cameraPos.parent->getLocalPos(pos.pos, pos.parent).z;
			if (depth > 0 && depth < config::cameraDepth)
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
			point2Dd drawPos = translatePositionToDisplay({ localPos.x,localPos.y }, zoom);
			HI2::drawTexture(*sprite.sprite, drawPos.x, drawPos.y, zoom, localPos.r);
			//HI2::drawRectangle({ (int)drawPos.x,(int)drawPos.y }, (int)config::spriteSize * zoom, (int)config::spriteSize * zoom, HI2::Color(0, 0, 0, 100));
		}
		void operator()(const nodeLayer& node) const {
			fdd firstBlock = node.node->getLocalPos(cameraPos.pos, cameraPos.parent); //bloc en que esta la camera
			firstBlock.z = node.layerHeight;

			fdd localPos = firstBlock - cameraPos.pos;

			firstBlock.x -= (HI2::getScreenWidth() / config::spriteSize) / 2;
			firstBlock.y -= (HI2::getScreenHeight() / config::spriteSize) / 2; // bloc del TL

			double fraccionalX = 0.5 - fmod(cameraPos.pos.x, 1);
			double fraccionalY = 0.5 - fmod(cameraPos.pos.y, 1);
			if (fraccionalX < 0)fraccionalX += 1;
			if (fraccionalY < 0)fraccionalY += 1;

			point2Dd drawPos = translatePositionToDisplay({ (double)-((HI2::getScreenWidth() / config::spriteSize) / 2) + fraccionalX,(double)-((HI2::getScreenHeight() / config::spriteSize) / 2) + fraccionalY }, zoom);

			for (int x = 0; x < HI2::getScreenWidth() / config::spriteSize; ++x)
			{
				int finalXdrawPos = drawPos.x + (x * zoom * config::spriteSize);
				if(finalXdrawPos+config::spriteSize * zoom < 0)
					continue;
				else if(finalXdrawPos > HI2::getScreenWidth())
					break;
				
				for (int y = 0; y < HI2::getScreenHeight() / config::spriteSize; ++y)
				{
					int finalYdrawPos = drawPos.y + (y * zoom * config::spriteSize);
					if(finalYdrawPos+config::spriteSize * zoom < 0)
						continue;
					else if(finalYdrawPos > HI2::getScreenHeight())
						break;
					
					block& b = node.node->getBlock({ (int)round(firstBlock.x) + x,(int)round(firstBlock.y) + y,node.layerHeight });
					if (b.visible){
						HI2::drawTexture(*b.texture, finalXdrawPos, finalYdrawPos, zoom, localPos.r);
						if constexpr(config::drawDepthShadows)
							HI2::drawRectangle({ finalXdrawPos,finalYdrawPos }, (int)config::spriteSize * zoom, (int)config::spriteSize * zoom, HI2::Color(0, 0, 0, 90*((config::depthScale-zoom)-config::minScale)));
					}
				}
			}
		}
		entt::registry* registry;
		position cameraPos;
		double zoom;
	};
	visitor v;
	v.registry = &_enttRegistry;
	v.cameraPos = _enttRegistry.get<position>(_camera);
	v.zoom = (((config::cameraDepth - rl.depth) / config::cameraDepth * (config::depthScale - config::minScale)) + config::minScale) * config::zoom;
	if (v.zoom > 0)
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

point2Dd State::Playing::translatePositionToDisplay(point2Dd pos, const double& zoom)
{
	pos.x *= config::spriteSize * zoom; // passem de coordenades del mon a coordenades de pantalla
	pos.y *= config::spriteSize * zoom;

	pos.x += (HI2::getScreenWidth() * zoom) / 2; //canviem el sistema de referencia respecte al centre (camera) a respecte el TL
	pos.y += (HI2::getScreenHeight() * zoom) / 2;

	pos.x -= (config::spriteSize * zoom) / 2; //dibuixem repecte el TL de la entitat, no pas la seva posicio (la  qual es el seu centre)
	pos.y -= (config::spriteSize * zoom) / 2;

	pos.x -= ((HI2::getScreenWidth() * zoom) - HI2::getScreenWidth()) / 2;
	pos.y -= ((HI2::getScreenHeight() * zoom) - HI2::getScreenHeight()) / 2;

	return pos;
}

