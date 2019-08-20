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
#include "entt/entity/helper.hpp"
#include "jsonTools.hpp"


universeNode* State::Playing::_chunkLoaderUniverseBase;
position* State::Playing::_chunkLoaderPlayerPosition;

State::Playing::~Playing() {
	
	saveGame();
	
	delete _chunkLoaderPlayerPosition;
	_chunkLoaderPlayerPosition = nullptr;
	_chunkLoaderThread->join();
	_universeBase.clean();
}

State::Playing::Playing(gameCore& gc, std::string saveName = "default", int seed = -1) :State_Base(gc) {

	_savePath = HI2::getSavesPath().append(saveName);

	//Create collision world
	rp3d::WorldSettings collisionSettings;
	collisionSettings.defaultVelocitySolverNbIterations = 20;
	collisionSettings.isSleepingEnabled = true;

	_collisionWorld = std::make_unique<rp3d::CollisionWorld>(collisionSettings);
	
	//create saveGame if it doesn't exist, otherwise load
	if (!std::filesystem::exists(savePath())) {
		if (seed == -1)
		{
			seed = rand();
		}
		createNewGame(seed);
	}
	else
	{
		loadGame();
	}

	
	auto playerView = _enttRegistry.view<entt::tag<"PLAYER"_hs>>();					   // Get camera and player
	for(auto entity: playerView) {															   //
	    _player = entity;																	   //
		_chunkLoaderPlayerPosition = new position(_enttRegistry.get<position>(entity));		   //
	}																						   //
	auto cameraView = _enttRegistry.view<entt::tag<"CAMERA"_hs>>();					   //
	for(auto entity: cameraView) {															   //
	    _camera = entity;																	   //
	}																						   //

	//start chunkloader
	_universeBase.updateChunks(_chunkLoaderPlayerPosition->pos, _chunkLoaderPlayerPosition->parent);
	_chunkLoaderUniverseBase = &_universeBase;
	_chunkLoaderThread = std::make_unique<std::thread>(_chunkLoaderFunc);
}

void State::Playing::input(float dt)
{
	auto& playerSpd = _enttRegistry.get<velocity>(_player);
	int held = HI2::getKeysHeld();

	if (held & HI2::BUTTON::KEY_MINUS) {
		playerSpd.spd = fdd();
	}
	if (held & HI2::BUTTON::KEY_UP) {
		playerSpd.spd.y -= 3 * dt;
	}
	if (held & HI2::BUTTON::KEY_DOWN) {
		playerSpd.spd.y += 3 * dt;
	}
	if (held & HI2::BUTTON::KEY_LEFT) {
		playerSpd.spd.x -= 3 * dt;
	}
	if (held & HI2::BUTTON::KEY_RIGHT) {
		playerSpd.spd.x += 3 * dt;
	}
	if (held & HI2::BUTTON::KEY_A) {
		playerSpd.spd.z += 6 * dt;
	}
	if (held & HI2::BUTTON::KEY_B) {
		playerSpd.spd.z -= 6 * dt;
	}
	if (held & HI2::BUTTON::KEY_PLUS) {
		_core->quit();
	}
	if (held & HI2::BUTTON::KEY_ZR) {
		config::zoom += 0.1 * dt;
	}
	if (held & HI2::BUTTON::KEY_ZL) {
		config::zoom -= 0.1 * dt;
	}
	if (held & HI2::BUTTON::KEY_R) {
		playerSpd.spd.r += dt;
	}
	if (held & HI2::BUTTON::KEY_L) {
		playerSpd.spd.r -= dt;
	}
}

void State::Playing::update(float dt) {

	//TODO update nodes positions

	auto movableEntityView = _enttRegistry.view<velocity, position>();
	for (const entt::entity& entity : movableEntityView) { //Update entities' positions
		velocity vel = movableEntityView.get<velocity>(entity);
		position& pos = movableEntityView.get<position>(entity);

		pos.pos += (vel.spd * dt);
	}

	//Test for collisions

	//Resolve collisions

	position& playerPosition = _enttRegistry.get<position>(_player);
	(*_chunkLoaderPlayerPosition) = playerPosition; // update chunkloader's player pos

	std::cout << "playerHeight: " << playerPosition.pos.z << std::endl;

	//Update camera to follow the player;
	position& cameraPosition = _enttRegistry.get<position>(_camera);
	cameraPosition.parent = playerPosition.parent;
	cameraPosition.pos.x = playerPosition.pos.x;
	cameraPosition.pos.y = playerPosition.pos.y;
	cameraPosition.pos.z = playerPosition.pos.z + (config::cameraDepth / 2);


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

			double tmp = fmod(cameraPos.pos.x, 1);
			if (tmp < 0)
				tmp = 1 - abs(tmp);
			double fraccionalX = 0.5 - tmp;
			if (fraccionalX < 0)fraccionalX += 1;

			tmp = fmod(cameraPos.pos.y, 1);
			if (tmp < 0)
				tmp = 1 - abs(tmp);
			double fraccionalY = 0.5 - tmp;
			if (fraccionalY < 0)fraccionalY += 1;

			point2Dd drawPos = translatePositionToDisplay({ (double)-((HI2::getScreenWidth() / config::spriteSize) / 2) + fraccionalX,(double)-((HI2::getScreenHeight() / config::spriteSize) / 2) + fraccionalY }, zoom);

			for (int x = 0; x < HI2::getScreenWidth() / config::spriteSize; ++x)
			{
				int finalXdrawPos = (int)(drawPos.x) + (x * zoom * config::spriteSize);
				if (finalXdrawPos + config::spriteSize * zoom < 0)
					continue;
				else if (finalXdrawPos > HI2::getScreenWidth())
					break;

				for (int y = 0; y < HI2::getScreenHeight() / config::spriteSize; ++y)
				{
					int finalYdrawPos = (int)(drawPos.y) + (y * zoom * config::spriteSize);
					if (finalYdrawPos + config::spriteSize * zoom < 0)
						continue;
					else if (finalYdrawPos > HI2::getScreenHeight())
						break;

					block& b = node.node->getBlock({ (int)round(firstBlock.x) + x,(int)round(firstBlock.y) + y,node.layerHeight });
					if (b.visible) {
						HI2::drawTexture(*b.texture, finalXdrawPos, finalYdrawPos, zoom, localPos.r);
						if constexpr (config::drawDepthShadows)
							HI2::drawRectangle({ finalXdrawPos,finalYdrawPos }, (int)config::spriteSize * zoom + 1, (int)config::spriteSize * zoom + 1, HI2::Color(0, 0, 0, 150 * (zoom / config::zoom > 1 ? -((zoom / config::zoom - 1) / (config::minScale - 1)) : ((zoom / config::zoom - 1) / (config::minScale - 1)))));
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
	std::ifstream terrainTableFile(HI2::getDataPath().append("blockTable.json"));
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

void State::Playing::createNewGame(int seed)
{
	std::filesystem::create_directories(savePath());
	std::cout << HI2::getDataPath().append("defData").append("universe.json") << std::endl;
	std::filesystem::copy_file(HI2::getDataPath().append("defData").append("universe.json"), savePath().append("universe.json"));

	//load universe.json
	std::ifstream universeFile(savePath().append("universe.json"));
	json j;
	universeFile >> j;
	j.get_to(_universeBase);
	_universeBase.linkChildren();

	//load terrain table
	loadTerrainTable();

	//Set up basic entities
	universeNode* result;
	_universeBase.findNodeByID(11, result);

	_player = _enttRegistry.create();
	_enttRegistry.assign<entt::tag<"PLAYER"_hs>>(_player);
	_camera = _enttRegistry.create();
	_enttRegistry.assign<entt::tag<"CAMERA"_hs>>(_camera);
	
	entt::entity dog = _enttRegistry.create();

	auto& playerSprite = _enttRegistry.assign<drawable>(_player);
	playerSprite.sprite = _core->getGraphics().loadTexture("player");
	playerSprite.name="player";

	auto& dogSprite = _enttRegistry.assign<drawable>(dog);
	dogSprite.sprite = _core->getGraphics().loadTexture("gromit");
	dogSprite.name="gromit";
	
	auto& dogPos = _enttRegistry.assign<position>(dog);
	dogPos.parent = result;
	dogPos.parentID=11;
	dogPos.pos.x = 0;
	dogPos.pos.y = 0;
	dogPos.pos.z = 1;
	dogPos.pos.r = 0;

	auto& dogSpd = _enttRegistry.assign<velocity>(dog);
	dogSpd.spd.x = 0;
	dogSpd.spd.y = 0;
	dogSpd.spd.z = 0;
	dogSpd.spd.r = -0.1;

	auto& playerPos = _enttRegistry.assign<position>(_player);
	playerPos.parent = result;
	playerPos.parentID=11;
	playerPos.pos.x = 0;
	playerPos.pos.y = 0;
	playerPos.pos.z = 1;
	playerPos.pos.r = 0;

	auto& playerSpd = _enttRegistry.assign<velocity>(_player);
	playerSpd.spd.x = 0;
	playerSpd.spd.y = 0;
	playerSpd.spd.z = 0;
	playerSpd.spd.r = 0.1;

	auto& cameraPos = _enttRegistry.assign<position>(_camera);
	cameraPos.parent = result;
	cameraPos.parentID=11;
	cameraPos.pos.x = 0;
	cameraPos.pos.y = 0;
	cameraPos.pos.z = 11;
	cameraPos.pos.r = 0;

	auto& cameraSpd = _enttRegistry.assign<velocity>(_camera);
	cameraSpd.spd.x = 0;
	cameraSpd.spd.y = 0;
	cameraSpd.spd.z = 0;
	cameraSpd.spd.r = 0;	
}

void State::Playing::loadGame()
{
	//load universe.json
	std::ifstream universeFile(savePath().append("universe.json"));
	json j;
	universeFile >> j;
	j.get_to(_universeBase);
	_universeBase.linkChildren();

	//load terrain table
	loadTerrainTable();

	loadEntities();
}

void State::Playing::saveGame()
{
	saveEntities();
	std::ofstream universeFile(savePath().append("universe.json"));
	nlohmann::json universeJson(_universeBase);
	universeJson >> universeFile;
}

void State::Playing::loadEntities()
{
	std::ifstream entitiesFile(savePath().append("entities.json"));
	nlohmann::json entitiesJson;
	entitiesFile >> entitiesJson;
	from_json(entitiesJson,_enttRegistry);
	fixEntities();
}

void State::Playing::saveEntities()
{
	std::ofstream entitiesFile(savePath().append("entities.json"));
	nlohmann::json entitiesJson;
	to_json(entitiesJson,_enttRegistry);
	entitiesJson >> entitiesFile;
}

void State::Playing::fixEntities()
{
	//position
	auto positionEntities = _enttRegistry.view<position>();
	for (const entt::entity& entity : positionEntities) {
		position& pos = _enttRegistry.get<position>(entity);
		if(!_universeBase.findNodeByID(pos.parentID,pos.parent))
		{
			throw "Node not found wtf";
		}
	}
	//drawable
	auto drawableEntities = _enttRegistry.view<drawable>();
	for (const entt::entity& entity : drawableEntities) {
		drawable& d = _enttRegistry.get<drawable>(entity);
		d.sprite=_core->getGraphics().loadTexture(d.name);
	}
	//body
}

void State::Playing::_chunkLoaderFunc()
{
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTick = std::chrono::high_resolution_clock::now();
	while (_chunkLoaderPlayerPosition != nullptr)
	{
		position p(*_chunkLoaderPlayerPosition); // aixo pot petar, hauria d usar algun lock o algo
		if (_chunkLoaderPlayerPosition != nullptr)
			_chunkLoaderUniverseBase->updateChunks(p.pos, p.parent);
		std::chrono::time_point<std::chrono::high_resolution_clock> currentTick = std::chrono::high_resolution_clock::now();
		auto microSeconds = std::chrono::duration_cast<std::chrono::microseconds>(currentTick - lastTick).count();
		if (microSeconds < 20000)
			std::this_thread::sleep_for(std::chrono::microseconds(20000));

		lastTick = currentTick;
	}
}

std::filesystem::path State::Playing::_savePath;

std::filesystem::path State::Playing::savePath() {
	return _savePath;
}