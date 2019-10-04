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
	int held = HI2::getKeysHeld();

	if (held & HI2::BUTTON::KEY_MINUS) {
		double oldR = playerSpd.spd.r;
		playerSpd.spd = fdd();
		playerSpd.spd.r = oldR;
	}
	if (held & HI2::BUTTON::KEY_LSTICK_UP) {
		playerSpd.spd.y -= 10 * dt;
	}
	if (held & HI2::BUTTON::KEY_LSTICK_DOWN) {
		playerSpd.spd.y += 10 * dt;
	}
	if (held & HI2::BUTTON::KEY_LSTICK_LEFT) {
		playerSpd.spd.x -= 10 * dt;
	}
	if (held & HI2::BUTTON::KEY_LSTICK_RIGHT) {
		playerSpd.spd.x += 10 * dt;
	}
	if (held & HI2::BUTTON::KEY_A) {
		playerSpd.spd.z += 60 * dt;
	}
	if (held & HI2::BUTTON::KEY_B) {
		playerSpd.spd.z -= 40 * dt;
	}
	if (held & HI2::BUTTON::KEY_PLUS) {
		playerPos.pos.z += 5;
		playerSpd.spd.z = 0;
	}
	if (held & HI2::BUTTON::KEY_ZR) {
		config::zoom += 10 * dt;
	}
	if (held & HI2::BUTTON::KEY_ZL) {
		config::zoom -= 10 * dt;
	}
	if (held & HI2::BUTTON::KEY_R) {
		playerSpd.spd.r += 10 * dt;
	}
	if (held & HI2::BUTTON::KEY_L) {
		playerSpd.spd.r -= 10 * dt;
	}
	if (held & HI2::BUTTON::KEY_Y) {
		playerPos.parent->setBlock({ &baseBlock::terrainTable[1],UP }, { (int)playerPos.pos.x,(int)playerPos.pos.y - 1,(int)playerPos.pos.z });
	}
	if (held & HI2::BUTTON::KEY_X) {
		playerPos.parent->setBlock({ &baseBlock::terrainTable[selectedBlock],selectedRotation,true }, { (int)playerPos.pos.x,(int)playerPos.pos.y - 1,(int)playerPos.pos.z });
	}
	if (held & HI2::BUTTON::KEY_DLEFT) {
		selectedBlock--;
		if (selectedBlock < 0)
			selectedBlock = baseBlock::terrainTable.size() - 1;
	}
	if (held & HI2::BUTTON::KEY_DRIGHT) {
		selectedBlock = (selectedBlock + 1) % baseBlock::terrainTable.size();
	}
	if (held & HI2::BUTTON::KEY_DUP) {
		selectedRotation++;
	}
	if (held & HI2::BUTTON::KEY_DDOWN) {
		selectedRotation--;
	}

}

void State::Playing::update(double dt) {

	_universeBase.updatePositions(dt);
	auto movableEntityView = _enttRegistry.view<velocity, position>();
	for (const entt::entity& entity : movableEntityView) { //Update entities' positions
		velocity& vel = movableEntityView.get<velocity>(entity);
		vel.spd.z -= 9.81 * dt;
		position& pos = movableEntityView.get<position>(entity);

		pos.pos += (vel.spd * dt);
	}


	//TODO update nodes positions
	_physicsEngine.dt = dt;
	Services::physicsMutex.lock();
#pragma region collision detection
#pragma region entity-entity
	{
		auto bodyEntitiesView = _enttRegistry.view<body>();
		for (const entt::entity& left : bodyEntitiesView) { //Update entities' positions
			for (const entt::entity& right : bodyEntitiesView) { //Update entities' positions
				if (left != right && left > right)
				{

					position pL = _enttRegistry.get<position>(left);
					position pR = _enttRegistry.get<position>(right);
					fdd rightPos = pL.parent->getLocalPos(pR.pos, pR.parent);

					rp3d::Vector3 leftPosition(pL.pos.x, pL.pos.y, pL.pos.z);
					rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
					rp3d::Transform leftTransform(leftPosition, initOrientation);

					rp3d::Vector3 rightPosition(rightPos.x, rightPos.y, rightPos.z);
					rp3d::Transform rightTransform(rightPosition, initOrientation);

					body& leftBody = _enttRegistry.get<body>(left);
					leftBody.collider->setTransform(leftTransform);
					body& rightBody = _enttRegistry.get<body>(right);
					rightBody.collider->setTransform(rightTransform);
					if (_physicsEngine.getWorld()->testAABBOverlap(leftBody.collider, rightBody.collider))
					{
						_physicsEngine.getWorld()->testCollision(leftBody.collider, rightBody.collider, &_physicsEngine);
					}
				}
			}
		}
	}
#pragma endregion
#pragma region node-entity
	{
		auto bodyEntitiesView = _enttRegistry.view<body>();
		for (const entt::entity& left : bodyEntitiesView) { //Update entities' positions
			position entityPos = _enttRegistry.get<position>(left);

			std::vector<universeNode*> collidableNodes = entityPos.parent->getParent()->getChildren();
			for (universeNode* ntemp : entityPos.parent->getChildren())
			{
				collidableNodes.push_back(ntemp);
			}
			collidableNodes.push_back(entityPos.parent->getParent());

			for (universeNode* node : collidableNodes)
			{
				position pos = _enttRegistry.get<position>(left);

				fdd posRelativeToNode = node->getLocalPos(pos.pos, pos.parent);
				collidedResponse cResponse;
				cResponse.type = NODE;
				collidedBody cBody;
				cBody.node = node;
				cResponse.body = cBody;

				rp3d::Vector3 entityPosition(posRelativeToNode.x, posRelativeToNode.y, posRelativeToNode.z);
				rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
				rp3d::Transform entityTransform(entityPosition, initOrientation);

				body& entityBody = _enttRegistry.get<body>(left);
				entityBody.collider->setTransform(entityTransform);

				if (_physicsEngine.getWorld()->testAABBOverlap(entityBody.collider, node->getNodeCollider()))
				{
					auto chunksToCheck = node->getTerrainColliders(posRelativeToNode, node);
					for (auto& chunk : chunksToCheck)
					{
						chunk->setUserData((void*)& cResponse);
						_physicsEngine.getWorld()->testCollision(entityBody.collider, chunk, &_physicsEngine);
					}
				}
			}


		}
	}
#pragma endregion
#pragma region node-node
#pragma endregion
	auto bodyEntitiesView = _enttRegistry.view<body>();
	for (const entt::entity& left : bodyEntitiesView) {
		_enttRegistry.get<body&>(left).lastCollided = nullptr;
	}

#pragma endregion 
	Services::physicsMutex.unlock();





	position& playerPosition = _enttRegistry.get<position>(_player);
	(*_chunkLoaderPlayerPosition) = playerPosition; // update chunkloader's player pos

	std::cout << std::fixed << std::setprecision(2) << "playerPos: " << std::setw(10) << playerPosition.pos.x << "x " << std::setw(10) << playerPosition.pos.y << "y " << std::setw(10) << playerPosition.pos.z << "z" << std::endl;

	//Update camera to follow the player;
	position& cameraPosition = _enttRegistry.get<position>(_camera);
	cameraPosition.parent = playerPosition.parent;
	cameraPosition.pos.x = playerPosition.pos.x;
	cameraPosition.pos.y = playerPosition.pos.y;
	cameraPosition.pos.z = playerPosition.pos.z + 5;
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
		HI2::drawTexture(*_core->getGraphics().getTexture(baseBlock::terrainTable[selectedBlock].name), 0, HI2::getScreenHeight() - config::spriteSize * 4, 4, ((double)(int)selectedRotation) * (M_PI / 2));
	HI2::drawText(_standardFont, std::to_string(double(1.0f / dt)), { 0,0 }, 30, dt > (1.0f / 29.0f) ? HI2::Color::Red : HI2::Color::Black);
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

					metaBlock* b = node.node->getBlock({ (int)round(firstBlock.x) + x,(int)round(firstBlock.y) + y,node.layerHeight });
					if (b != nullptr && b->base->visible) {
						if constexpr (config::drawDepthShadows) {
							//	HI2::drawRectangle({ finalXdrawPos,finalYdrawPos }, (int)config::spriteSize * zoom + 1, (int)config::spriteSize * zoom + 1, HI2::Color(0, 0, 0, 150 * (zoom / config::zoom > 1 ? -((zoom / config::zoom - 1) / (config::minScale - 1)) : ((zoom / config::zoom - 1) / (config::minScale - 1)))));

							//mask anira de 255 a 150
							HI2::setTextureColorMod(*b->base->texture, HI2::Color(mask, mask, mask, 0));
							HI2::drawTexture(*b->base->texture, finalXdrawPos, finalYdrawPos, zoom, ((double)(int)b->rotation) * (M_PI / 2));
						}
						else
						{
							HI2::drawTexture(*b->base->texture, finalXdrawPos, finalYdrawPos, zoom, localPos.r + b->rotation);
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
	std::cout << HI2::getDataPath().append("defData").append("universe.json") << std::endl;
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
	int pID = 11;
	bool temp = _universeBase.findNodeByID(pID, result);


	{
		_player = _enttRegistry.create();
		_enttRegistry.assign<entt::tag<"PLAYER"_hs>>(_player);

		auto& playerSprite = _enttRegistry.assign<drawable>(_player);
		playerSprite.sprite = _core->getGraphics().loadTexture("player");
		playerSprite.name = "player";

		auto& playerPos = _enttRegistry.assign<position>(_player);
		playerPos.parent = result;
		playerPos.parentID = pID;
		playerPos.pos.x = 2 + 8;
		playerPos.pos.y = 2 + 8;
		playerPos.pos.z = 2 + 8;
		playerPos.pos.r = 0;

		auto& playerSpd = _enttRegistry.assign<velocity>(_player);
		playerSpd.spd.x = 0;
		playerSpd.spd.y = 0;
		playerSpd.spd.z = 0;
		playerSpd.spd.r = 0.1;

		auto& playerName = _enttRegistry.assign<name>(_player);
		playerName.nameString = "Captain Lewis";

		auto& playerBody = _enttRegistry.assign<body>(_player);
		playerBody.height = 0.9;
		playerBody.width = 0.8;
		playerBody.mass = 50;

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
		dogPos.pos.z = 2 + 8;
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
			ballPos.pos.z = i + j + 4 + 8;
			ballPos.pos.r = 0;

			auto& ballSpd = _enttRegistry.assign<velocity>(ball);
			ballSpd.spd.x = 0;
			ballSpd.spd.y = 0;
			ballSpd.spd.z = 0;
			ballSpd.spd.r = -0.1;

			auto& ballBody = _enttRegistry.assign<body>(ball);
			ballBody.height = 7.0f / 8.0f;
			ballBody.width = 7.0f / 8.0f;
			ballBody.mass = 0.1;

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