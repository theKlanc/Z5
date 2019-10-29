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
#include "HardwareInterface/HardwareInterface.hpp"
universeNode* State::Playing::_chunkLoaderUniverseBase;
position* State::Playing::_chunkLoaderPlayerPosition;
std::mutex State::Playing::endChunkLoader;
bool lungsFull = true;


State::Playing::~Playing() {


	//delete _chunkLoaderPlayerPosition;
	endChunkLoader.lock();
	{
		_chunkLoaderPlayerPosition = nullptr;
		_chunkLoaderThread->join();
	}
	endChunkLoader.unlock();
	saveGame();
	_universeBase.clean();
}

State::Playing::Playing(gameCore& gc, std::string saveName = "default", int seed = -1) :State_Base(gc), _standardFont("data/fonts/test.ttf") {

	Services::enttRegistry = &_enttRegistry;
	Services::collisionWorld = _physicsEngine.getWorld();
	_savePath = HI2::getSavesPath().append(saveName);

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
	//_universeBase.populateColliders(_physicsEngine.getWorld());

	auto playerView = _enttRegistry.view<entt::tag<"PLAYER"_hs>>();					   // Get camera and player
	for (auto entity : playerView) {															   //
		_player = entity;																	   //
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
}

void State::Playing::input(double dt)
{
	auto& playerSpd = _enttRegistry.get<velocity>(_player);
	auto& playerPos = _enttRegistry.get<position>(_player);
	auto& playerBdy = _enttRegistry.get<body>(_player);
	unsigned long long held = HI2::getKeysHeld();
	unsigned long long down = HI2::getKeysDown();

	//STOP
	if (held & HI2::BUTTON::KEY_MINUS) {
		playerSpd.spd = fdd();
	}

	//MOVE
	if (held & (HI2::BUTTON::KEY_LSTICK_UP | HI2::BUTTON::KEY_W)) {
		playerSpd.spd.y -= 10 * dt;
	}
	if (held & (HI2::BUTTON::KEY_LSTICK_DOWN | HI2::BUTTON::KEY_S)) {
		playerSpd.spd.y += 10 * dt;
	}
	if (held & (HI2::BUTTON::KEY_LSTICK_LEFT | HI2::BUTTON::KEY_A)) {
		playerSpd.spd.x -= 10 * dt;
	}
	if (held & (HI2::BUTTON::KEY_LSTICK_RIGHT | HI2::BUTTON::KEY_D)) {
		playerSpd.spd.x += 10 * dt;
	}

	//MOVE VERTICALLY
	if (held & HI2::BUTTON::KEY_R) {
		playerSpd.spd.z += 60 * dt;
	}
	if (held & HI2::BUTTON::KEY_F) {
		playerSpd.spd.z -= 40 * dt;
	}

	//TELEPORT UPWARDS
	if (down & HI2::BUTTON::KEY_PLUS) {
		playerPos.pos.z += 5;
		playerSpd.spd.z = 0;
	}


	//ROTATE PLAYER
	if (held & HI2::BUTTON::KEY_E) {
		playerSpd.spd.r += 10 * dt;
	}
	if (held & HI2::BUTTON::KEY_Q) {
		playerSpd.spd.r -= 10 * dt;
	}

	//PLACE BLOCK
	if (down & HI2::BUTTON::KEY_P) {
		playerPos.parent->setBlock({ &baseBlock::terrainTable[1],UP }, { (int)playerPos.pos.x,(int)playerPos.pos.y - 1,(int)playerPos.pos.z });
	}
	if (down & HI2::BUTTON::KEY_O) {
		playerPos.parent->setBlock({ &baseBlock::terrainTable[selectedBlock],selectedRotation,true }, { (int)playerPos.pos.x,(int)playerPos.pos.y - 1,(int)playerPos.pos.z });
	}

	//SELECT BLOCK
	if (down & HI2::BUTTON::KEY_DLEFT) {
		selectedBlock--;
		if (selectedBlock < 0)
			selectedBlock = baseBlock::terrainTable.size() - 1;
	}
	if (down & HI2::BUTTON::KEY_DRIGHT) {
		selectedBlock = (selectedBlock + 1) % baseBlock::terrainTable.size();
	}

	//BLOCK ROTATE
	if (down & HI2::BUTTON::KEY_DUP) {
		selectedRotation++;
	}
	if (down & HI2::BUTTON::KEY_DDOWN) {
		selectedRotation--;
	}

	//TOGGLE GRAVITY
	if (down & HI2::BUTTON::KEY_G)
	{
		config::gravityEnabled = !config::gravityEnabled;
		std::cout << "Gravity " << (config::gravityEnabled ? "En" : "Dis") << "abled" << std::endl;
	}

	//TOGGLE DRAG
	if (down & HI2::BUTTON::KEY_C)
	{
		config::dragEnabled = !config::dragEnabled;
		std::cout << "Drag " << (config::dragEnabled ? "En" : "Dis") << "abled" << std::endl;
	}

	//CAMERA ZOOM
	if (held & HI2::BUTTON::KEY_ZR) {
		config::zoom += dt;
		std::cout << "Zoom: " << config::zoom << std::endl;
	}
	if (held & HI2::BUTTON::KEY_ZL) {
		config::zoom /= 1.01;
		std::cout << "Zoom: " << config::zoom << std::endl;
	}

	//CAMERA DEPTH
	if (down & HI2::BUTTON::KEY_H)
	{
		config::cameraDepth++;
		std::cout << "CameraDepth: " << config::cameraDepth << std::endl;
	}
	if (down & HI2::BUTTON::KEY_B)
	{
		config::cameraDepth--;
		std::cout << "CameraDepth: " << config::cameraDepth << std::endl;
	}

	// CAMERA HEIGHT
	if (down & HI2::BUTTON::KEY_U)
	{
		config::cameraHeight--;
		std::cout << "CameraHeight: " << config::cameraHeight << std::endl;
	}
	if (down & HI2::BUTTON::KEY_I)
	{
		config::cameraHeight++;
		std::cout << "CameraHeight: " << config::cameraHeight << std::endl;
	}

	// minScale
	if (down & HI2::BUTTON::KEY_J)
	{
		config::minScale += 0.05;
		std::cout << "MinScale: " << config::minScale << std::endl;
	}
	if (down & HI2::BUTTON::KEY_N)
	{
		config::minScale -= 0.05;
		std::cout << "MinScale: " << config::minScale << std::endl;
	}

	// Scale
	if (down & HI2::BUTTON::KEY_K)
	{
		config::depthScale += 0.05;
		std::cout << "DepthScale: " << config::depthScale << std::endl;
	}
	if (down & HI2::BUTTON::KEY_M)
	{
		config::depthScale -= 0.05;
		std::cout << "DepthScale: " << config::depthScale << std::endl;
	}

	// Shadow
	if (held & HI2::BUTTON::KEY_T)
	{
		config::minShadow--;
		std::cout << "minShadow: " << config::minShadow << std::endl;
	}
	if (held & HI2::BUTTON::KEY_Y)
	{
		config::minShadow++;
		std::cout << "minShadow: " << config::minShadow << std::endl;
	}

	// Fullscreen
	if (down & HI2::BUTTON::KEY_F11)
	{
		HI2::toggleFullscreen();
	}

	// Breathe
	if (down & HI2::BUTTON::KEY_X)
	{
		if (lungsFull)
		{
			lungsFull = !lungsFull;
			playerBdy.volume -= 0.01;
			std::cout << "Exhale" << std::endl;
		}
		else
		{
			metaBlock* block = playerPos.parent->getBlock({ (int)playerPos.pos.x,(int)playerPos.pos.y,(int)(playerPos.pos.z + playerBdy.height) });
			if (block != nullptr && block->base->name == "air")
			{
				lungsFull = !lungsFull;
				playerBdy.volume += 0.01;
				std::cout << "Inhale" << std::endl;
			}
		}
	}
}

void State::Playing::update(double dt) {

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
	cameraPosition.pos.z = playerPosition.pos.z + config::cameraHeight;
	if (_enttRegistry.has<body>(_player))
	{
		cameraPosition.pos.z += _enttRegistry.get<body>(_player).height;
	}
}

void State::Playing::draw(double dt) {
	_core->getGraphics().stepAnimations(dt * 1000);
	std::vector<renderLayer> renderOrders;
	HI2::setBackgroundColor(HI2::Color(20, 5, 100, 255));
	{
		position cameraPos = _enttRegistry.get<position>(_camera);
		std::vector<universeNode*> sortedDrawingNodes = _universeBase.nodesToDraw(cameraPos.pos, cameraPos.parent);
		for (universeNode*& node : sortedDrawingNodes) {
			std::vector<bool> visibility((HI2::getScreenWidth() / config::spriteSize) * HI2::getScreenHeight() / config::spriteSize, true);
			for (int i = 0; i <= config::cameraDepth; ++i) {//for depth afegim cada capa dels DrawingNodes
				position currentCameraPos = cameraPos;
				currentCameraPos.pos.z -= i;
				//obtenir posicio de la camera al node
				fdd localCameraPos = node->getLocalPos(currentCameraPos.pos, currentCameraPos.parent);
				//obtenir profunditat
				int layer = floor(localCameraPos.z);

				double partFraccional = fmod(localCameraPos.z, 1);
				double depth = i + partFraccional;

				nodeLayer nLayer = generateNodeLayer(node, depth, visibility, localCameraPos);

				renderOrders.push_back(renderLayer{ depth,std::variant<entt::entity,nodeLayer>(nLayer) });
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
	if (baseBlock::terrainTable[selectedBlock].visible)
	{
		HI2::setTextureColorMod(*_core->getGraphics().getTexture(baseBlock::terrainTable[selectedBlock].name), HI2::Color(255, 255, 255, 0));
		HI2::drawTexture(*_core->getGraphics().getTexture(baseBlock::terrainTable[selectedBlock].name), 0, HI2::getScreenHeight() - config::spriteSize * 4, 4, ((double)(int)selectedRotation) * (M_PI / 2));
	}
	position playerPos = _enttRegistry.get<position>(_player);
	HI2::drawText(_standardFont, std::to_string(double(1.0f / dt)), { 0,0 }, 30, dt > (1.0f / 29.0f) ? HI2::Color::Red : HI2::Color::Black);
	HI2::drawText(_standardFont, "ID: " + std::to_string(playerPos.parent->getID()), { 0,30 }, 30, HI2::Color::Orange);
	HI2::drawText(_standardFont, "X: " + std::to_string(playerPos.pos.x), { 0,60 }, 30, HI2::Color::Pink);
	HI2::drawText(_standardFont, "Y: " + std::to_string(playerPos.pos.y), { 0,90 }, 30, HI2::Color::Green);
	HI2::drawText(_standardFont, "Z: " + std::to_string(playerPos.pos.z), { 0,120 }, 30, HI2::Color::Yellow);
	HI2::drawText(_standardFont, "R: " + std::to_string(playerPos.pos.r), { 0,150 }, 30, HI2::Color::Orange);

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
			const drawable& sprite = registry->get<drawable>(entity);
			const position& entityPosition = registry->get<position>(entity);
			fdd localPos = cameraPos.parent->getLocalPos(entityPosition.pos, entityPosition.parent) - cameraPos.pos;
			point2Dd drawPos = translatePositionToDisplay({ localPos.x,localPos.y }, zoom);
			if constexpr (config::drawDepthShadows) {
				HI2::setTextureColorMod(*sprite.sprite, HI2::Color(mask, mask, mask, 0));
			}
			HI2::drawTexture(*sprite.sprite, drawPos.x, drawPos.y, zoom, localPos.r);
			//HI2::drawRectangle({ (int)drawPos.x,(int)drawPos.y }, (int)config::spriteSize * zoom, (int)config::spriteSize * zoom, HI2::Color(0, 0, 0, 100));
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
				tmp = 1 - abs(tmp);
			double fraccionalX = 0.5 - tmp;
			if (fraccionalX < 0)fraccionalX += 1;

			tmp = fmod(firstBlock.y, 1);
			if (tmp < 0)
				tmp = 1 - abs(tmp);
			double fraccionalY = 0.5 - tmp;
			if (fraccionalY < 0)fraccionalY += 1;

			fdd localPos = firstBlock - cameraPos.pos;

			firstBlock.x -= (HI2::getScreenWidth() / config::spriteSize) / 2;
			firstBlock.y -= (HI2::getScreenHeight() / config::spriteSize) / 2; // bloc del TL


			const point2Dd drawPos = translatePositionToDisplay({ (double)-((HI2::getScreenWidth() / config::spriteSize) / 2) + fraccionalX,(double)-((HI2::getScreenHeight() / config::spriteSize) / 2) + fraccionalY }, zoom);

			for (int x = 0; x < HI2::getScreenWidth() / config::spriteSize; ++x)
			{
				const int finalXdrawPos = (int)(drawPos.x) + (x * zoom * config::spriteSize);
				if (finalXdrawPos + config::spriteSize * zoom < 0)
					continue;
				else if (finalXdrawPos > HI2::getScreenWidth())
					break;

				for (int y = 0; y < HI2::getScreenHeight() / config::spriteSize; ++y)
				{
					const int finalYdrawPos = (int)(drawPos.y) + (y * zoom * config::spriteSize);
					if (finalYdrawPos + config::spriteSize * zoom < 0)
						continue;
					else if (finalYdrawPos > HI2::getScreenHeight())
						break;
					const int index = x * HI2::getScreenHeight() / config::spriteSize + y;

					metaBlock* b = node.node->getBlock({ (int)round(firstBlock.x) + x,(int)round(firstBlock.y) + y,node.layerHeight });
					if (node.visibility[index] && b != nullptr) {
						metaBlock bCopy = *b;
						if (bCopy.base->visible)
						{
							if constexpr (config::drawDepthShadows) {
								//mask anira de 255 a 150
								HI2::setTextureColorMod(*bCopy.base->texture, HI2::Color(mask, mask, mask, 0));
								HI2::drawTexture(*bCopy.base->texture, finalXdrawPos, finalYdrawPos, zoom, ((double)(int)b->rotation) * (M_PI / 2));
							}
							else {
								HI2::drawTexture(*b->base->texture, finalXdrawPos, finalYdrawPos, zoom, localPos.r + b->rotation);
							}
						}
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
	for (int x = 0; x < HI2::getScreenWidth() / config::spriteSize; ++x)
	{
		for (int y = 0; y < HI2::getScreenHeight() / config::spriteSize; ++y)
		{
			metaBlock* b = node->getBlock({ (int)round(firstBlock.x) + x,(int)round(firstBlock.y) + y,layerHeight });
			result.blocks.push_back(b);
			visibility[i] = (b == nullptr ? true : !b->base->opaque);
			i++;
		}
	}
	visibility = growVisibility(visibility);
	return result;
}

std::vector<bool> State::Playing::growVisibility(std::vector<bool> visibility)
{
	std::vector<bool> newVis(visibility);
	int rowSize = HI2::getScreenHeight() / config::spriteSize;
	if constexpr (true) {
		std::vector<int> rows;
		for (int i = 0; i < HI2::getScreenWidth() / config::spriteSize; ++i)
			rows.push_back(i);

		auto testFunc = [rowSize, visibility, &newVis](const int& n)
		{
			for (int y = 0; y < HI2::getScreenHeight() / config::spriteSize; ++y)
			{
				int index = n * HI2::getScreenHeight() / config::spriteSize + y;
				if (visibility[index])
				{
					if (n > 0)
						newVis[index - rowSize] = true;
					if (n < HI2::getScreenWidth() / config::spriteSize - 1)
						newVis[index + rowSize] = true;
					if (y > 0)
						newVis[index - 1] = true;
					if (y < HI2::getScreenHeight() / config::spriteSize - 1)
						newVis[index + 1] = true;

					if (n > 0 && y > 0)
						newVis[index - rowSize - 1] = true;
					if (n < HI2::getScreenWidth() / config::spriteSize - 1 && y < HI2::getScreenHeight() / config::spriteSize - 1)
						newVis[index + rowSize + 1] = true;
					if (y > 0 && n < HI2::getScreenWidth() / config::spriteSize - 1)
						newVis[index - 1 + rowSize] = true;
					if (y < HI2::getScreenHeight() / config::spriteSize - 1 && n > 0)
						newVis[index + 1 - rowSize] = true;
				}
			}
		};
		std::for_each(rows.begin(), rows.end(), testFunc);
	}
	else {
		for (int x = 0; x < HI2::getScreenWidth() / config::spriteSize; ++x)
		{
			for (int y = 0; y < HI2::getScreenHeight() / config::spriteSize; ++y)
			{
				int index = x * HI2::getScreenHeight() / config::spriteSize + y;
				if (visibility[index])
				{
					if (x > 0)
						newVis[index - rowSize] = true;
					if (x < HI2::getScreenWidth() / config::spriteSize - 1)
						newVis[index + rowSize] = true;
					if (y > 0)
						newVis[index - 1] = true;
					if (y < HI2::getScreenHeight() / config::spriteSize - 1)
						newVis[index + 1] = true;

					if (x > 0 && y > 0)
						newVis[index - rowSize - 1] = true;
					if (x < HI2::getScreenWidth() / config::spriteSize - 1 && y < HI2::getScreenHeight() / config::spriteSize - 1)
						newVis[index + rowSize + 1] = true;
					if (y > 0 && x < HI2::getScreenWidth() / config::spriteSize - 1)
						newVis[index - 1 + rowSize] = true;
					if (y < HI2::getScreenHeight() / config::spriteSize - 1 && x > 0)
						newVis[index + 1 - rowSize] = true;
				}
			}
		}
	}
	return newVis;
}

void State::Playing::loadTerrainTable()
{
	std::ifstream terrainTableFile(HI2::getDataPath().append("blockTable.json"));
	json j;
	terrainTableFile >> j;
	j.get_to(_terrainTable);
	for (baseBlock& b : _terrainTable) {
		if (b.visible) {
			b.texture = _core->getGraphics().loadTexture(b.name);
		}
	}
	baseBlock::terrainTable = _terrainTable;
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
	std::filesystem::copy_file(HI2::getDataPath().append("defData").append("universe.json"), savePath().append("universe.json"));

	//load terrain table
	loadTerrainTable();

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
	//load terrain table
	loadTerrainTable();

	//load universe.json
	std::ifstream universeFile(savePath().append("universe.json"));
	json j;
	universeFile >> j;
	j.get_to(_universeBase);
	_universeBase.linkChildren();



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
	entitiesJson >> entitiesFile;
}

void State::Playing::createEntities()
{
	//Set up basic entities
	universeNode* result;
	int pID = 4;
	bool temp = _universeBase.findNodeByID(pID, result);


	{
		_player = _enttRegistry.create();
		_enttRegistry.assign<entt::tag<"PLAYER"_hs>>(_player);

		auto& playerSprite = _enttRegistry.assign<drawable>(_player);
		playerSprite.sprite = _core->getGraphics().loadTexture("player2");
		playerSprite.name = "player";

		auto& playerPos = _enttRegistry.assign<position>(_player);
		playerPos.parent = result;
		playerPos.parentID = pID;
		playerPos.pos.x = 0;
		playerPos.pos.y = 0;
		playerPos.pos.z = 260;
		playerPos.pos.r = 0;

		auto& playerSpd = _enttRegistry.assign<velocity>(_player);
		playerSpd.spd.x = 0;
		playerSpd.spd.y = 0;
		playerSpd.spd.z = 0;
		playerSpd.spd.r = 0;

		auto& playerName = _enttRegistry.assign<name>(_player);
		playerName.nameString = "Captain Lewis";

		auto& playerBody = _enttRegistry.assign<body>(_player);
		playerBody.height = 0.9;
		playerBody.width = 0.8;
		playerBody.mass = 65;
		playerBody.volume = 0.07;
		playerBody.elasticity = 0.3;

		// Initial position and orientation of the collision body 
		rp3d::Vector3 initPosition(0.0, 0.0, 0.0);
		rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(initPosition, initOrientation);

		playerBody.collider = _physicsEngine.getWorld()->createCollisionBody(transform);
		collidedResponse* playerResponse = new collidedResponse();
		playerResponse->type = ENTITY;
		playerResponse->body.entity = _player;
		playerBody.collider->setUserData((void*)playerResponse);
		initPosition.z += playerBody.width / 2;
		transform.setPosition(initPosition);
		playerBody._collisionShape = new rp3d::SphereShape(playerBody.width / 2);
		playerBody.collider->addCollisionShape(playerBody._collisionShape, transform);
	}

	{
		_camera = _enttRegistry.create();
		_enttRegistry.assign<entt::tag<"CAMERA"_hs>>(_camera);
		_enttRegistry.assign<position>(_camera);
		_enttRegistry.assign<entt::tag<"PLAYER"_hs>>(_camera);
	}
	{
		entt::entity dog = _enttRegistry.create();

		auto& dogSprite = _enttRegistry.assign<drawable>(dog);
		dogSprite.sprite = _core->getGraphics().loadTexture("dog");
		dogSprite.name = "dog";

		auto& dogPos = _enttRegistry.assign<position>(dog);
		dogPos.parent = result;
		dogPos.parentID = pID;
		dogPos.pos.x = 4 + 8;
		dogPos.pos.y = 4 + 8;
		dogPos.pos.z = 260;
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
		dogBody.elasticity = 0.3;
		dogBody.volume = 0.02;

		// Initial position and orientation of the collision body 
		rp3d::Vector3 initPosition(0.0, 0.0, 0.0);
		rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(initPosition, initOrientation);

		dogBody.collider = _physicsEngine.getWorld()->createCollisionBody(transform);
		collidedResponse* dogResponse = new collidedResponse();
		dogResponse->type = ENTITY;
		dogResponse->body.entity = dog;
		dogBody.collider->setUserData((void*)dogResponse);
		initPosition.z += dogBody.width / 2;
		transform.setPosition(initPosition);
		dogBody._collisionShape = new rp3d::SphereShape(dogBody.width / 2);
		dogBody.collider->addCollisionShape(dogBody._collisionShape, transform);
	}
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 5; j++)
		{
			entt::entity ball = _enttRegistry.create();

			auto& ballSprite = _enttRegistry.assign<drawable>(ball);
			ballSprite.sprite = _core->getGraphics().loadTexture("ball");
			ballSprite.name = "ball";

			auto& ballPos = _enttRegistry.assign<position>(ball);
			ballPos.parent = result;
			ballPos.parentID = pID;
			ballPos.pos.x = 4 + i + 8;
			ballPos.pos.y = 4 + j + 8;
			ballPos.pos.z = 260 + i + j + 4 + 8;
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

			ballBody.collider = _physicsEngine.getWorld()->createCollisionBody(transform);

			collidedResponse* ballResponse = new collidedResponse();
			ballResponse->type = ENTITY;
			ballResponse->body.entity = ball;
			ballBody.collider->setUserData((void*)ballResponse);
			ballBody._collisionShape = new rp3d::SphereShape(0.4);
			initPosition.z += ballBody.width / 2;
			transform.setPosition(initPosition);
			ballBody.collider->addCollisionShape(ballBody._collisionShape, transform);
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
	auto drawableEntities = _enttRegistry.view<drawable>();
	for (const entt::entity& entity : drawableEntities) {
		drawable& d = _enttRegistry.get<drawable>(entity);
		d.sprite = _core->getGraphics().loadTexture(d.name);
	}
	//body
	auto bodyEntities = _enttRegistry.view<body>();
	for (const entt::entity& entity : bodyEntities) {
		body& b = _enttRegistry.get<body>(entity);
		position p = _enttRegistry.get<position>(entity);

		rp3d::Vector3 initPosition(0, 0, 0);
		rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
		rp3d::Transform transform(initPosition, initOrientation);

		b.collider = _physicsEngine.getWorld()->createCollisionBody(transform);

		collidedResponse* bodyResponse = new collidedResponse();
		bodyResponse->type = ENTITY;
		bodyResponse->body.entity = entity;
		b.collider->setUserData((void*)bodyResponse);

		b._collisionShape = new rp3d::CapsuleShape(b.width / 2, b.height);
		b.collider->addCollisionShape(b._collisionShape, transform);
	}
}

void State::Playing::_chunkLoaderFunc()
{
	while (_chunkLoaderPlayerPosition != nullptr) {
		endChunkLoader.lock();
		position p(*_chunkLoaderPlayerPosition); // aixo pot petar, hauria d usar algun lock o algo
		if (_chunkLoaderPlayerPosition != nullptr)
			_chunkLoaderUniverseBase->updateChunks(p.pos, p.parent);
		endChunkLoader.unlock();
	}
}

std::filesystem::path State::Playing::_savePath;

std::filesystem::path State::Playing::savePath() {
	return _savePath;
}