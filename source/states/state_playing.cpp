#include "components/brain.hpp"
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
#include "components/body.hpp"
#include "components/name.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"
#include "physicsEngine.hpp"
#include "components/astronautBrain.hpp"
#include <cmath>
#include "HI2.hpp"
#include "nodeGenerators/terrainPainterGenerator.hpp"
#include "nodeGenerators/prefabGenerator.hpp"
#include "components/brain.hpp"
#include "UI/customGadgets/starmap.hpp"
#include <memory>
#include "fuel.hpp"

universeNode* State::Playing::_chunkLoaderUniverseBase;
position* State::Playing::_chunkLoaderPlayerPosition;
std::mutex State::Playing::_chunkLoaderMutex;

State::Playing::~Playing() {


	_chunkLoaderPlayerPosition = nullptr;
	_chunkLoaderThread->join();

	saveGame();
	_universeBase.clean();
}

State::Playing::Playing(gameCore& gc, std::string saveName, int seed, bool debug) :State_Base(gc), _standardFont(*Services::fonts.loadFont("lemon")) {
	_debug = debug;

	//load json tables
	baseBlock::loadTerrainTable();
	fuel::loadFuelList();

	Services::lcg.seed(seed);
	Services::enttRegistry = &_enttRegistry;
	Services::collisionWorld = _physicsEngine.getWorld();
	_savePath = HI2::getSavesPath().append(saveName);

	////DEBUG SECTION
	//terrainPainterGenerator tpg(4,12742000);
	//std::cout << "JSON\n" << tpg.getJson() << "ENDJSON\n" << std::flush;
	////ENDEBUG

	//create saveGame if it doesn't exist, otherwise load
	if (!std::filesystem::exists(savePath())) {
		if (seed == -1)
		{
			seed = rand();
		}
		createNewGame(saveName, seed);
	}
	else
	{
		loadGame();
	}

	if (_debug) {
		_console = std::make_shared<basicTextEntry>(point2D{ 0,0 }, point2D{ HI2::getScreenWidth(),40 }, _standardFont, 35, "", "Enter a command here", HI2::Color(0, 0, 0, 127), HI2::Color(255, 255, 255, 255));
		_console->toggle();
		_console->setCallback(std::bind(&State::Playing::debugConsoleExec, this, std::placeholders::_1));
		_scene.addGadget(_console);
	}

	auto playerView = _enttRegistry.view<entt::tag<"PLAYER"_hs>>();					   // Get camera and player
	universeNode* playerParent;
	for (auto entity : playerView) {															   //
		_player = entity;
		playerParent = _enttRegistry.get<position>(entity).parent;															   //
		_chunkLoaderPlayerPosition = new position(_enttRegistry.get<position>(entity));		   //
	}																						   //
	auto cameraView = _enttRegistry.view<entt::tag<"CAMERA"_hs>>();					   //
	for (auto entity : cameraView) {															   //
		_camera = entity;																	   //
	}																						   //

	//start chunkloader
	_universeBase.updateChunks(_chunkLoaderPlayerPosition->pos, _chunkLoaderPlayerPosition->parent);
	_chunkLoaderUniverseBase = &_universeBase;
	_chunkLoaderThread = std::make_unique<std::thread>(_chunkLoaderFunc);
	_starmap = std::make_shared<starmap>(point2D{50,50},point2D{800,600},&_universeBase,playerParent);
	_starmap->toggle();
	_scene.addGadget(_starmap);
}

void State::Playing::input(double dt)
{
	_step = false;

	auto& playerSpd = _enttRegistry.get<velocity>(_player);
	auto& playerPos = _enttRegistry.get<position>(_player);
	const std::bitset<HI2::BUTTON_SIZE>& held = HI2::getKeysHeld();
	std::bitset<HI2::BUTTON_SIZE> down = HI2::getKeysDown();
	const std::bitset<HI2::BUTTON_SIZE>& up = HI2::getKeysUp();
	const point2D& mouse = HI2::getTouchPos();

	if (_debug && down[HI2::BUTTON::KEY_CONSOLE]) {
		_console->toggle();
	}
	if(_console->isActive()){
		_scene.select(_console);
	}
	else {
		if (_debug && down[HI2::BUTTON::KEY_Z]) {
			_step = true;
		}
		if (down[HI2::BUTTON::KEY_M]) {
			_starmap->toggle();
			_scene.select(_starmap);
		}
		// Exit
		if (held[HI2::BUTTON::CANCEL])
		{
			_core->popState();
		}
	}
	_scene.update(down, up, held, mouse, dt);
	_enttRegistry.get<std::unique_ptr<brain>>(_player)->update(dt,down,up,held);
}

void State::Playing::update(double dt) {
	if (_paused)
		dt = 0;
	if (_step)
		dt = 1.0f / config::physicsHz;
	//update enemies
	auto brainEntities = _enttRegistry.view<std::unique_ptr<brain>>();
	for (auto entity : brainEntities) {
		if(entity != _player)
			brainEntities.get<std::unique_ptr<brain>>(entity)->update(dt);
	}

	//Update thrusters
	for(auto& node : _universeBase){
		node.updateThrusters(dt);
	}

	//TODO update nodes positions
	_physicsEngine.processCollisions(_universeBase, _enttRegistry, dt);

	position& playerPosition = _enttRegistry.get<position>(_player);
	(*_chunkLoaderPlayerPosition) = playerPosition; // update chunkloader's player pos

	//std::cout << std::fixed << std::setprecision(2) << "playerPos: " << std::setw(10) << playerPosition.pos.x << "x " << std::setw(10) << playerPosition.pos.y << "y " << std::setw(10) << playerPosition.pos.z << "z" << std::endl;

	//Update camera to follow the player;
	position& cameraPosition = _enttRegistry.get<position>(_camera);
	cameraPosition.parent = playerPosition.parent;
	cameraPosition.pos.x = playerPosition.pos.x;
	cameraPosition.pos.y = playerPosition.pos.y;
	cameraPosition.pos.z = playerPosition.pos.z + 0.01;

	if (_enttRegistry.has<body>(_player))
	{
		cameraPosition.pos.z += _enttRegistry.get<body>(_player).height;
		int h = 0;
		for (int i = 0; i < config::cameraHeight; i++)
		{
			fdd newCameraPos = cameraPosition.pos;
			newCameraPos.z = newCameraPos.z + h;
			if (!cameraPosition.parent->getBlock(newCameraPos.getPoint3Di()).base->opaque)
			{
				h++;
			}
			else
			{
				break;
			}
		}
		cameraPosition.pos.z += h;
	}
}

void State::Playing::draw(double dt) {
	if (_paused)
		dt = 0;
	if (_step)
		dt = 1.0f / config::physicsHz;

	Services::graphics.stepAnimations(dt);
	
	position playerPos = _enttRegistry.get<position>(_player);
	velocity playerVel = _enttRegistry.get<velocity>(_player);

	bool interactableInRange = playerPos.parent->getClosestInteractable(playerPos.pos) != nullptr;
	point3Di iblePos = playerPos.parent->getClosestInteractablePos(playerPos.pos);

	std::vector<renderLayer> renderOrders;
	HI2::setBackgroundColor(HI2::Color(0, 0, 0, 255));{
		position cameraPos = _enttRegistry.get<position>(_camera);
		std::vector<universeNode*> sortedDrawingNodes = _universeBase.nodesToDraw(cameraPos.pos, cameraPos.parent);
		for (universeNode*& node : sortedDrawingNodes) {
			std::vector<bool> visibility(((int)(HI2::getScreenWidth() / config::spriteSize)) * ((int)(HI2::getScreenHeight() / config::spriteSize)), true);
			for (int i = 0; i <= config::cameraDepth; ++i) {//for depth afegim cada capa dels DrawingNodes
				position currentCameraPos = cameraPos;
				currentCameraPos.pos.z -= i;
				//obtenir posicio de la camera al node
				fdd localCameraPos = node->getLocalPos(currentCameraPos.pos, currentCameraPos.parent);
				//obtenir profunditat
				int layer = floor(localCameraPos.z);

				double partFraccional = fmod(localCameraPos.z, 1);
				double depth = i + partFraccional - 1;

				if(depth < 0.2)
					continue;
				depth +=0.5;
				nodeLayer nLayer = generateNodeLayer(node, depth, visibility, localCameraPos);
				renderOrders.push_back(renderLayer{ depth,std::variant<entt::entity,nodeLayer,point3Di>(nLayer) });

				if(interactableInRange && node == playerPos.parent && iblePos.z == layer){
					renderOrders.push_back(renderLayer{ depth-0.001,std::variant<entt::entity,nodeLayer,point3Di>(iblePos)});
				}
			}
		}

		auto drawableEntityView = _enttRegistry.view<drawable, position>();
		for (auto entity : drawableEntityView) { // afegim les entitats dibuixables
			auto& pos = drawableEntityView.get<position>(entity);
			double depth = cameraPos.pos.z - cameraPos.parent->getLocalPos(pos.pos, pos.parent).z;
			if (_enttRegistry.has<body>(entity))
			{
				depth -= _enttRegistry.get<body>(entity).height;
			}
			if (depth > 0 && depth < config::cameraDepth)
				renderOrders.push_back(renderLayer{ depth - 0.01,	std::variant<entt::entity,nodeLayer,point3Di>(entity) });
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
	if (baseBlock::terrainTable[selectedBlock].visible)
	{
		sprite& s = *Services::graphics.getSprite(baseBlock::terrainTable[selectedBlock].name);
		HI2::setTextureColorMod(*s.getTexture(), HI2::Color(255, 255, 255, 0));
		HI2::drawTexture(*s.getTexture(), 0, HI2::getScreenHeight() - config::spriteSize * 4, s.getCurrentFrame().size, s.getCurrentFrame().startPos, 4, ((double)(int)selectedRotation) * (M_PI / 2), selectedFlip ? HI2::FLIP::H : HI2::FLIP::NONE);
	}
	auto& br = _enttRegistry.get<std::unique_ptr<brain>>(_player);
	if (_debug) {
		HI2::drawText(_standardFont, std::to_string(double(1.0f / dt)), { 0,0 }, 30, dt > (1.0f / 29.0f) ? HI2::Color::Red : HI2::Color::Black);
		HI2::drawText(_standardFont, "Parent: " + playerPos.parent->getName() + " (" + std::to_string(playerPos.parent->getID()) + ")", { 0,30 }, 30, HI2::Color::Orange);
		HI2::drawText(_standardFont, "X: " + std::to_string(playerPos.pos.x), { 0,60 }, 30, HI2::Color::Pink);
		HI2::drawText(_standardFont, "Y: " + std::to_string(playerPos.pos.y), { 0,90 }, 30, HI2::Color::Green);
		HI2::drawText(_standardFont, "Z: " + std::to_string(playerPos.pos.z), { 0,120 }, 30, HI2::Color::Yellow);
		HI2::drawText(_standardFont, "R: " + std::to_string(playerPos.pos.r), { 0,150 }, 30, HI2::Color::Orange);
		HI2::drawText(_standardFont, "vx: " + std::to_string(playerVel.spd.x), { 0,180 }, 30, HI2::Color::Red);
		HI2::drawText(_standardFont, "vy: " + std::to_string(playerVel.spd.y), { 0,210 }, 30, HI2::Color::Green);
		HI2::drawText(_standardFont, "vz: " + std::to_string(playerVel.spd.z), { 0,240 }, 30, HI2::Color::Blue);
		HI2::drawText(_standardFont, "vr: " + std::to_string(playerVel.spd.r), { 0,270 }, 30, HI2::Color::Pink);
		HI2::drawText(_standardFont, "Thoughts: " + br->getThoughts(), { 0,300 }, 30, HI2::Color::Black);
		
		//HI2::drawText(_standardFont, "insideBlock: " + std::to_string(playerPos.parent->getBlock({(int)floor(playerPos.pos.x),(int)floor(playerPos.pos.y),(int)floor(playerPos.pos.z + 0.3)}).base->ID), { 0,300 }, 30, HI2::Color::Black);
	}
	_scene.draw();
	_enttRegistry.get<std::unique_ptr<brain>>(_player)->drawUI();
	HI2::endFrame();
}

void State::Playing::drawLayer(const State::Playing::renderLayer& rl)
{
	struct visitor {
		void operator()(const entt::entity& entity) const {

			double depthFactor = ((zoom / config::zoom) - config::minScale) / (config::depthScale - config::minScale);
			if (depthFactor < 0)
				depthFactor = 0;
			//depthFactor=1-pow(1-depthFactor,2);
			int topVis = 255 - config::minShadow;
			double shadowVal = depthFactor * topVis;
			short mask = shadowVal + config::minShadow;
			const drawable& drw = registry->get<drawable>(entity);
			const position& entityPosition = registry->get<position>(entity);
			fdd localPos = cameraPos.parent->getLocalPos(entityPosition.pos, entityPosition.parent) - cameraPos.pos;
			point2Dd drawPos = translatePositionToDisplay({ localPos.x,localPos.y }, zoom);
			//late culling
			if(drawPos.x <= HI2::getScreenWidth() && drawPos.y <= HI2::getScreenHeight() && (drawPos.x + zoom*drw.spr->getCurrentFrame().size.x>=0) && (drawPos.y + zoom*drw.spr->getCurrentFrame().size.y>=0))
			{
				if (config::drawDepthShadows) {
					HI2::setTextureColorMod(*drw.spr->getTexture(), HI2::Color(mask, mask, mask, 0));
				}
				HI2::drawTexture(*drw.spr->getTexture(), drawPos.x, drawPos.y, drw.spr->getCurrentFrame().size, drw.spr->getCurrentFrame().startPos, zoom, localPos.r, HI2::FLIP::NONE);
				//HI2::drawRectangle({ (int)drawPos.x,(int)drawPos.y }, (int)config::spriteSize * zoom, (int)config::spriteSize * zoom, HI2::Color(0, 0, 0, 100));
			}
		}
		void operator()(const nodeLayer& node) const {
			//      deep - - - - shallow
			// dFt  0                  1
			// shdw 0                105
			// mask 150              255
			// 
			double depthFactor = ((zoom / config::zoom) - config::minScale) / (config::depthScale - config::minScale);
			if (depthFactor < 0)
				depthFactor = 0;
			//depthFactor=pow(depthFactor,2);
			int topVis = 255 - config::minShadow;
			double shadowVal = depthFactor * topVis;
			short mask = shadowVal + config::minShadow;


			fdd firstBlock = node.node->getLocalPos(cameraPos.pos, cameraPos.parent); //bloc on esta la camera
			firstBlock.z = node.layerHeight;

			double tmp = fmod(firstBlock.x, 1);

			if (tmp < 0)
				tmp = 1.0f - std::abs(tmp);
			double fraccionalX = 0.5 - tmp;

			if (fraccionalX < 0)fraccionalX += 1;

			tmp = fmod(firstBlock.y, 1);
			if (tmp < 0)
				tmp = 1.0f - std::abs(tmp);
			double fraccionalY = 0.5 - tmp;
			if (fraccionalY < 0)fraccionalY += 1;
			fdd localPos = firstBlock - cameraPos.pos;

			int rowSize = (HI2::getScreenWidth() / config::spriteSize);
			int colSize = (HI2::getScreenHeight() / config::spriteSize);

			firstBlock.x -= rowSize / 2;
			firstBlock.y -= colSize / 2; // bloc del TL

			point2Dd drawPos = translatePositionToDisplay({ (double)-((HI2::getScreenWidth() / config::spriteSize) / 2) + fraccionalX,(double)-((HI2::getScreenHeight() / config::spriteSize) / 2) + fraccionalY }, zoom);
			for (int x = 0; x < HI2::getScreenWidth() / config::spriteSize; ++x)
			{
				const int finalXdrawPos = (int)(drawPos.x) + (x * zoom * config::spriteSize);
				if (finalXdrawPos + config::spriteSize * zoom < 0) //Netejar aquesta porcada, reimplementar el que es va fer a la branca de RTT
					continue;
				else if (finalXdrawPos > HI2::getScreenWidth())
					break;

				for (int y = 0; y < colSize; ++y)
				{
					if (node.visibility[(y * rowSize) + x]) {
						const int finalYdrawPos = (int)(drawPos.y) + (y * zoom * config::spriteSize);
						if (finalYdrawPos + config::spriteSize * zoom < 0)
							continue;
						else if (finalYdrawPos > HI2::getScreenHeight())
							break;

						metaBlock& b = node.node->getBlock({ (int)round(firstBlock.x) + x,(int)round(firstBlock.y) + y,node.layerHeight });
						if (b.base->ID != 0 && b.base->visible) {
							if (config::drawDepthShadows) {
								//mask anira de 255 a 150
								HI2::setTextureColorMod(*b.base->spr->getTexture(), HI2::Color(mask, mask, mask, 0));
							}
							//HI2::drawRectangle({finalXdrawPos, finalYdrawPos},16.0*zoom,16.0*zoom,node.node->getMainColor());
							HI2::drawTextureOverlap(*b.base->spr->getTexture(), finalXdrawPos, finalYdrawPos, b.base->spr->getCurrentFrame().size, b.base->spr->getCurrentFrame().startPos, zoom, ((double)(int)b.rotation) * (M_PI / 2), b.flip ? HI2::FLIP::H : HI2::FLIP::NONE);
						}
					}
				}
			}
		}
		void operator()(const point3Di& p) const {
			point2Dd drawPos = translatePositionToDisplay({p.x-cameraPos.pos.x+0.5,p.y-cameraPos.pos.y+0.5}, zoom);
			HI2::drawEmptyRectangle({(int)drawPos.x,(int)drawPos.y},16*zoom,16*zoom,3,HI2::getKeysHeld()[HI2::BUTTON::KEY_ENTER]?HI2::Color::Green : HI2::Color::White);
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

State::Playing::nodeLayer State::Playing::generateNodeLayer(universeNode* node, double depth, std::vector<bool>& visibility, fdd localCameraPos)
{
	fdd firstBlock = localCameraPos;
	firstBlock.z = floor(localCameraPos.z);

	firstBlock.x -= (HI2::getScreenWidth() / config::spriteSize) / 2;
	firstBlock.y -= (HI2::getScreenHeight() / config::spriteSize) / 2; // bloc del TL

	const int layerHeight = floor(localCameraPos.z);

	nodeLayer result;
	result.node = node;
	result.layerHeight = layerHeight;
	result.visibility = visibility;
	int i = 0;
	for (int y = 0; y < floor(HI2::getScreenHeight() / config::spriteSize); ++y)
	{
		for (int x = 0; x < floor(HI2::getScreenWidth() / config::spriteSize); ++x)
		{
			metaBlock b = node->getBlock({ (int)round(firstBlock.x) + x,(int)round(firstBlock.y) + y,layerHeight });
			result.blocks.push_back(b);
			visibility[i] = (b.base->ID == 0 ? true : !b.base->opaque);
			i++;
		}
	}
	visibility = growVisibility(visibility);
	visibility = growVisibility(visibility);

	return result;
}

std::vector<bool> State::Playing::growVisibility(std::vector<bool> visibility)
{
	std::vector<bool> newVis(visibility);
	int rowSize = HI2::getScreenWidth() / config::spriteSize;
	int colSize = HI2::getScreenHeight() / config::spriteSize;

	for (int y = 0; y < colSize; ++y)
	{
		for (int x = 0; x < rowSize; ++x)
		{
			int index = x + (y * rowSize);
			//visibility[index] = true;
			if (visibility[index])
			{
				if (x > 0 && y > 0)//UL
					newVis[index - rowSize - 1] = true;
				if (y > 0) //UP
					newVis[index - rowSize] = true;
				if (y > 0 && x < rowSize - 1)//UR
					newVis[index - rowSize + 1] = true;
				if (x > 0) //LEFT
					newVis[index - 1] = true;
				if (x < rowSize - 1) //RIGHT
					newVis[index + 1] = true;
				if (y < colSize - 1 && x > 0)//DL
					newVis[index - 1 + rowSize] = true;
				if (y < colSize - 1) //DOWN
					newVis[index + rowSize] = true;
				if (x < rowSize - 1 && y < colSize - 1) //DR
					newVis[index + rowSize + 1] = true;
			}
		}
	}
	return newVis;
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

void State::Playing::createNewGame(std::string saveName, int seed)
{
	std::filesystem::create_directory(HI2::getSavesPath().append(saveName));
	std::filesystem::copy_file(HI2::getDataPath().append("defData").append("universe.json"), savePath().append("universe.json"));

	//load universe.json
	std::ifstream universeFile(savePath().append("universe.json"));
	json j;
	universeFile >> j;
	j.get_to(_universeBase);
	_universeBase.linkChildren();



	createEntities();
}

void State::Playing::loadGame()
{
	//load universe.json
	std::ifstream universeFile(savePath().append("universe.json"));
	json j;
	universeFile >> j;
	j.get_to(_universeBase);
	_universeBase.linkChildren();

	loadEntities();

	_debug = true;
}

void State::Playing::saveGame()
{
	saveEntities();
	std::ofstream universeFile(savePath().append("universe.json"));
	nlohmann::json universeJson(_universeBase);
	universeFile << universeJson;
}

void State::Playing::loadEntities()
{
	if (std::filesystem::exists(savePath().append("entities.json"))) {
		std::ifstream entitiesFile(savePath().append("entities.json"));
		nlohmann::json entitiesJson;
		entitiesFile >> entitiesJson;
		from_json(entitiesJson, _enttRegistry);
		fixEntities();
	}
	else
	{
		createEntities();
	}
}

void State::Playing::saveEntities() const
{
	std::ofstream entitiesFile(savePath().append("entities.json"));
	nlohmann::json entitiesJson;
	to_json(entitiesJson, _enttRegistry);
	entitiesFile << entitiesJson;
}

void State::Playing::createEntities()
{
	//Set up basic entities
	universeNode* result;
	{
		int pID = 4;
		bool temp = _universeBase.findNodeByID(pID, result);
	}

	double angle = Services::lcg();
	angle = angle / Services::lcg.max() * (2 * M_PI);
	double distance = Services::lcg() % ((int)result->getDiameter() / 2);
	while ((double)result->getHeight({ (int)(sin(angle) * distance),(int)(cos(angle) * distance) }) < 250)
	{
		angle = Services::lcg();
		angle = angle / Services::lcg.max() * (2 * M_PI);
		distance = Services::lcg() % ((int)result->getDiameter() / 2);
	}

	universeNode spaceShip("test_plat", 100000, 128, { sin(angle) * distance - 10.2,cos(angle) * distance + 0.2,(double)result->getHeight({(int)(sin(angle) * distance),(int)(cos(angle) * distance)}) + 20, 0 }, { 2,2,0 }, { 0,0,0 }, nodeType::SPACESHIP, result, 200);
	spaceShip.connectGenerator(std::make_unique<prefabGenerator>("test"));
	result->addChild(spaceShip);
	//result = result->getChildren()[1];
	{
		_player = _enttRegistry.create();
		_enttRegistry.assign<entt::tag<"PLAYER"_hs>>(_player);

		auto& playerSprite = _enttRegistry.assign<drawable>(_player);
		std::vector<frame> playerFrames;
		playerFrames.push_back({ {256,0},{16,16} });
		playerFrames.push_back({ {256,16},{16,16} });
		playerFrames.push_back({ {256,32},{16,16} });
		playerFrames.push_back({ {256,48},{16,16} });
		playerFrames.push_back({ {256,64},{16,16} });
		playerFrames.push_back({ {256,80},{16,16} });
		playerFrames.push_back({ {272,0},{16,16} });
		playerFrames.push_back({ {272,16},{16,16} });
		playerFrames.push_back({ {272,32},{16,16} });
		playerFrames.push_back({ {272,48},{16,16} });
		playerFrames.push_back({ {272,64},{16,16} });
		playerFrames.push_back({ {272,80},{16,16} });
		playerSprite.spr = Services::graphics.loadSprite("player3", "spritesheet", playerFrames);
		playerSprite.name = "player3";


		auto& playerPos = _enttRegistry.assign<position>(_player);
		playerPos.parent = result;
		playerPos.parentID = result->getID();
		playerPos.pos.x = sin(angle) * distance;
		playerPos.pos.y = cos(angle) * distance;
		playerPos.pos.z = result->getHeight(playerPos.pos.getPoint2D()) + 1;
		playerPos.pos.r = 0;

		auto& playerSpd = _enttRegistry.assign<velocity>(_player);
		playerSpd.spd.x = 0;
		playerSpd.spd.y = 0;
		playerSpd.spd.z = 0;
		playerSpd.spd.r = 0;

		auto& playerName = _enttRegistry.assign<name>(_player);
		playerName.nameString = "Captain Lewis";

		auto& playerBody = _enttRegistry.assign<body>(_player);
		playerBody.height = 0.8;
		playerBody.width = 0.8;
		playerBody.mass = 65;
		playerBody.volume = 0.07;
		playerBody.elasticity = 0.1;

		// Initial position and orientation of the collision body 
		rp3d::Vector3 initPosition(0.0, 0.0, 0.0);
		rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(initPosition, initOrientation);

		playerBody.physicsData.collider = _physicsEngine.getWorld()->createCollisionBody(transform);
		collidedResponse* playerResponse = new collidedResponse();
		playerResponse->type = physicsType::ENTITY;
		playerResponse->body.entity = _player;
		playerBody.physicsData.collider->setUserData((void*)playerResponse);
		initPosition = rp3d::Vector3(0, 0, playerBody.width / 2);
		transform.setPosition(initPosition);
		playerBody.physicsData._collisionShape = new rp3d::SphereShape(playerBody.width / 2);
		playerBody.physicsData.collider->addCollisionShape(playerBody.physicsData._collisionShape, transform);

		_enttRegistry.assign<std::unique_ptr<brain>>(_player) = std::make_unique<astronautBrain>(_player);
	}

	{
		_camera = _enttRegistry.create();
		_enttRegistry.assign<entt::tag<"CAMERA"_hs>>(_camera);
		_enttRegistry.assign<position>(_camera);
	}
	bool constexpr skipExtraEntities = false;
	if constexpr(!skipExtraEntities){
		{
			entt::entity dog = _enttRegistry.create();

			auto& dogSprite = _enttRegistry.assign<drawable>(dog);
			std::vector<frame> dogFrames;
			dogFrames.push_back({ {0,32},{16,16} });
			dogSprite.spr = Services::graphics.loadSprite("dog", "spritesheet", dogFrames);
			dogSprite.name = "dog";

			auto& dogPos = _enttRegistry.assign<position>(dog);
			dogPos.parent = result;
			dogPos.parentID = result->getID();
			dogPos.pos.x = 2 + sin(angle) * distance;
			dogPos.pos.y = 2 + cos(angle) * distance;
			dogPos.pos.z = result->getHeight(dogPos.pos.getPoint2D()) + 1;
			dogPos.pos.r = 0;

			auto& dogSpd = _enttRegistry.assign<velocity>(dog);
			dogSpd.spd.x = 0;
			dogSpd.spd.y = 0;
			dogSpd.spd.z = 0;
			dogSpd.spd.r = -0.1;

			auto& dogName = _enttRegistry.assign<name>(dog);
			dogName.nameString = "Lieutenant Gromit";

			auto& dogBody = _enttRegistry.assign<body>(dog);
			dogBody.height = 0.4;
			dogBody.width = 0.3;
			dogBody.mass = 10;
			dogBody.elasticity = 0.1;
			dogBody.volume = 0.02;

			// Initial position and orientation of the collision body
			rp3d::Vector3 initPosition(0.0, 0.0, 0.0);
			rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
			rp3d::Transform transform(initPosition, initOrientation);

			dogBody.physicsData.collider = _physicsEngine.getWorld()->createCollisionBody(transform);
			collidedResponse* dogResponse = new collidedResponse();
			dogResponse->type = physicsType::ENTITY;
			dogResponse->body.entity = dog;
			dogBody.physicsData.collider->setUserData((void*)dogResponse);
			initPosition.z += dogBody.width / 2;
			transform.setPosition(initPosition);
			dogBody.physicsData._collisionShape = new rp3d::SphereShape(dogBody.width / 2);
			dogBody.physicsData.collider->addCollisionShape(dogBody.physicsData._collisionShape, transform);
		}
		for (int i = 0; i < 5; i++){
			for (int j = 0; j < 5; j++)
			{
				entt::entity ball = _enttRegistry.create();

				auto& ballSprite = _enttRegistry.assign<drawable>(ball);
				std::vector<frame> dogFrames;
				if (Services::graphics.isSpriteLoaded("ball")) {
					ballSprite.spr = Services::graphics.getSprite("ball");
				}
				else {
					std::vector<frame> ballFrames;
					ballFrames.push_back({ {240,0},{16,16} });
					ballSprite.spr = Services::graphics.loadSprite("ball", "spritesheet", ballFrames);
					ballSprite.spr = Services::graphics.loadSprite("ball");
				}

				ballSprite.name = "ball";

				auto& ballPos = _enttRegistry.assign<position>(ball);
				ballPos.parent = result;
				ballPos.parentID = result->getID();
				ballPos.pos.x = 4 + i + sin(angle) * distance;
				ballPos.pos.y = 4 + j + cos(angle) * distance;
				ballPos.pos.z = result->getHeight(ballPos.pos.getPoint2D()) + i + j + 4;
				ballPos.pos.r = 0;

				auto& ballSpd = _enttRegistry.assign<velocity>(ball);
				ballSpd.spd.x = 0;
				ballSpd.spd.y = 0;
				ballSpd.spd.z = 0;
				ballSpd.spd.r = -0.1;

				auto& ballBody = _enttRegistry.assign<body>(ball);
				ballBody.height = 7.0f / 8.0f;
				ballBody.width = 7.0f / 8.0f;
				ballBody.mass = 1;
				ballBody.elasticity = 0.98;
				ballBody.volume = 0.2;

				// Initial position and orientation of the collision body
				rp3d::Vector3 initPosition(0.0, 0.0, 0.0);
				rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
				rp3d::Transform transform(initPosition, initOrientation);

				ballBody.physicsData.collider = _physicsEngine.getWorld()->createCollisionBody(transform);

				collidedResponse* ballResponse = new collidedResponse();
				ballResponse->type = physicsType::ENTITY;
				ballResponse->body.entity = ball;
				ballBody.physicsData.collider->setUserData((void*)ballResponse);
				ballBody.physicsData._collisionShape = new rp3d::SphereShape(0.4);
				initPosition.z += ballBody.width / 2;
				transform.setPosition(initPosition);
				ballBody.physicsData.collider->addCollisionShape(ballBody.physicsData._collisionShape, transform);
			}
		}
	}
}

void State::Playing::fixEntities()
{
	//position
	auto positionEntities = _enttRegistry.view<position>();
	for (const entt::entity& entity : positionEntities) {
		position& pos = _enttRegistry.get<position>(entity);
		if (!_universeBase.findNodeByID(pos.parentID, pos.parent))
		{
			throw "Node not found wtf";
		}
	}
	//drawable
	//auto drawableEntities = _enttRegistry.view<drawable>();
	//for (const entt::entity& entity : drawableEntities) {
	//	drawable& d = _enttRegistry.get<drawable>(entity);
	//	d.spr = Services::graphics.loadSprite(d.name);
	//}
	//body
	auto bodyEntities = _enttRegistry.view<body>();
	for (const entt::entity& entity : bodyEntities) {
		body& b = _enttRegistry.get<body>(entity);

		rp3d::Vector3 initPosition(0, 0, 0);
		rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(initPosition, initOrientation);

		b.physicsData.collider = _physicsEngine.getWorld()->createCollisionBody(transform);

		collidedResponse* bodyResponse = new collidedResponse();
		bodyResponse->type = physicsType::ENTITY;
		bodyResponse->body.entity = entity;
		b.physicsData.collider->setUserData((void*)bodyResponse);

		b.physicsData._collisionShape = new rp3d::SphereShape(b.width / 2);
		initPosition = rp3d::Vector3(0, 0, b.width / 2);
		transform.setPosition(initPosition);
		b.physicsData.collider->addCollisionShape(b.physicsData._collisionShape, transform);
	}
}

void State::Playing::_chunkLoaderFunc()
{
	position* positionCopy = _chunkLoaderPlayerPosition;
	while (positionCopy != nullptr) {
		_chunkLoaderMutex.lock();
		position p(*positionCopy);
		if (positionCopy != nullptr)
		{
			_chunkLoaderUniverseBase->updateChunks(p.pos, p.parent);
		}
		positionCopy = _chunkLoaderPlayerPosition;
		_chunkLoaderMutex.unlock();
	}
}

std::filesystem::path State::Playing::_savePath;

std::filesystem::path State::Playing::savePath() {
	return _savePath;
}

void State::Playing::debugConsoleExec(std::string input)
{
	std::stringstream ss(input);
	std::string command;
	ss >> command;
	if (command == "help") { //fake switch
		std::cout << "tp [x] [y] [z]" << std::endl;
		std::cout << "listNodes" << std::endl;
		std::cout << "nodeInfo ID" << std::endl;
		std::cout << "stop" << std::endl;
		std::cout << "setParent ID" << std::endl;
		std::cout << "toggleGravity" << std::endl;
		std::cout << "toggleDrag" << std::endl;
		std::cout << "toggleDepthShadows" << std::endl;
		std::cout << "setNodePos ID x y z" << std::endl;
		std::cout << "setNodeVel ID x y z" << std::endl;
		std::cout << "pause" << std::endl;
		std::cout << "step" << std::endl;
		std::cout << "setNullBlock ID" << std::endl;
		std::cout << "zoom zoomLevel" << std::endl;
		std::cout << "goto ID" << std::endl;

	}
	else if (command == "pause") {
		_paused = !_paused;
	}
	else if (command == "step") {
		_step = true;
	}
	else if (command == "stop") {
		velocity& vel = _enttRegistry.get<velocity>(_player);
		vel.spd = fdd();
	}
	else if (command == "toggleGravity") {
		config::gravityEnabled = !config::gravityEnabled;
	}
	else if (command == "toggleDrag") {
		config::dragEnabled = !config::dragEnabled;
	}
	else if (command == "toggleDepthShadows") {
		config::drawDepthShadows = !config::drawDepthShadows;
	}
	else if (command == "listNodes") {
		for (universeNode& node : _universeBase) {
			std::string sep;
			for (int i = 0; i < node.getDepth(); ++i) {
				sep += "   ";
			}
			std::cout << sep << "ID: " << node.getID() << std::endl;
			std::cout << sep << "name: " << node.getName() << std::endl;
			std::cout << sep << "type: " << node.getType() << std::endl;
			std::cout << sep << "pos: " << node.getPosition() << std::endl;

		}
	}
	else if (command == "nodeInfo" && ss.tellg() != -1) {
		std::string argument;
		ss >> argument;
		unsigned id = std::strtol(argument.c_str(), nullptr, 10);
		universeNode* node;
		if (_universeBase.findNodeByID(id, node)) {
			std::string sep;
			for (int i = 0; i < node->getDepth(); ++i) {
				sep += "   ";
			}
			std::cout << sep << "ID: " << node->getID() << std::endl;
			std::cout << sep << "name: " << node->getName() << std::endl;
			std::cout << sep << "type: " << node->getType() << std::endl;
			std::cout << sep << "pos: " << node->getPosition() << std::endl;
		}
	}
	else if (command == "setZoom" && ss.tellg() != -1) {
		std::string argument;
		ss >> argument;
		config::zoom = std::stoi(argument);
	}
	else if (command == "setNodePos" && ss.tellg() != -1) {
		std::string argument;
		ss >> argument;
		unsigned id = std::strtol(argument.c_str(), nullptr, 10);
		universeNode* node;
		if (_universeBase.findNodeByID(id, node)) {
			fdd pos;
			if (ss.tellg() != -1) {
				ss >> argument;
				pos.x = std::strtol(argument.c_str(), nullptr, 10);
			}
			if (ss.tellg() != -1) {
				ss >> argument;
				pos.y = std::strtol(argument.c_str(), nullptr, 10);
			}
			if (ss.tellg() != -1) {
				ss >> argument;
				pos.z = std::strtol(argument.c_str(), nullptr, 10);
			}
			node->setPosition(pos);
		}
	}
	else if (command == "setNodeVel" && ss.tellg() != -1) {
		std::string argument;
		ss >> argument;
		unsigned id = std::strtol(argument.c_str(), nullptr, 10);
		universeNode* node;
		if (_universeBase.findNodeByID(id, node)) {
			fdd pos;
			if (ss.tellg() != -1) {
				ss >> argument;
				pos.x = std::strtol(argument.c_str(), nullptr, 10);
			}
			if (ss.tellg() != -1) {
				ss >> argument;
				pos.y = std::strtol(argument.c_str(), nullptr, 10);
			}
			if (ss.tellg() != -1) {
				ss >> argument;
				pos.z = std::strtol(argument.c_str(), nullptr, 10);
			}
			node->setVelocity(pos);
			node->physicsData.sleeping = false;
		}
	}
	else if (command == "setParent" && ss.tellg() != -1) {
		std::string argument;
		ss >> argument;
		unsigned id = std::strtol(argument.c_str(), nullptr, 10);
		universeNode* node;
		if (_universeBase.findNodeByID(id, node)) {
			velocity& vel = _enttRegistry.get<velocity>(_player);
			position& pos = _enttRegistry.get<position>(_player);
			vel.spd = node->getLocalVel(vel.spd, pos.parent);
			pos.pos = node->getLocalPos(pos.pos, pos.parent);
			pos.parent = node;
			pos.parentID = node->getID();
		}
	}
	else if (command == "tp") {
		velocity& vel = _enttRegistry.get<velocity>(_player);
		vel.spd = fdd();
		position& pos = _enttRegistry.get<position>(_player);
		std::string argument;
		if (ss.tellg() != -1) {
			ss >> argument;
			pos.pos.x = std::strtol(argument.c_str(), nullptr, 10);
		}
		if (ss.tellg() != -1) {
			ss >> argument;
			pos.pos.y = std::strtol(argument.c_str(), nullptr, 10);
		}
		if (ss.tellg() != -1) {
			ss >> argument;
			pos.pos.z = std::strtol(argument.c_str(), nullptr, 10);
		}
		else {
			pos.pos.z = 1 + pos.parent->getHeight({ (int)pos.pos.x,(int)pos.pos.y });
		}
	}
	else if (command == "goto" && ss.tellg() != -1){
		std::string argument;
		ss >> argument;
		debugConsoleExec("setParent " + argument);
		debugConsoleExec("tp 0 0");
		position pos = _enttRegistry.get<position>(_player);
		_chunkLoaderMutex.lock();
		_universeBase.updateChunks(pos.pos,pos.parent);
		_chunkLoaderMutex.unlock();
	}
	else if (command == "setNullBlock") { // Y THO
		std::string argument;
		if (ss.tellg() != -1) {
			ss >> argument;
			metaBlock::nullBlock.base = &baseBlock::terrainTable[std::strtol(argument.c_str(), nullptr, 10)];
		}
	}
	else if (command == "awaken") { // Y THO
		std::string argument;
		ss >> argument;
		unsigned id = std::strtol(argument.c_str(), nullptr, 10);
		universeNode* node;
		if (_universeBase.findNodeByID(id, node)) {
			node->physicsData.sleeping=false;
		}
	}

	std::cout << input << std::endl;
}