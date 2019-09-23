#pragma once
#include "state_base.hpp"
#include "universeNode.hpp"
#include "entt/entity/registry.hpp"
#include "block.hpp"
#include <variant>
#include <vector>
#include <filesystem>
#include <thread>
#include "components/position.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"
#include "physicsEngine.hpp"

namespace State {
	class Playing : public virtual State_Base {
	  public:
		~Playing();
		Playing(gameCore &gc, std::string saveName, int seed);

		void input(float dt) override;
		void update(float dt) override;
		void draw() override;
		
		static std::filesystem::path savePath();

		
		

		
	  private:

		
		struct nodeLayer{
			universeNode* node;
			int layerHeight;
			std::vector<block*> blocks;
			std::vector<bool> visibility;
		};
		struct renderLayer{
			double depth;
			std::variant<entt::entity,nodeLayer> target;
		};

		nodeLayer generateNodeLayer(universeNode* node, double depth, std::vector<bool>& visibility, fdd localCameraPos);
		std::vector<bool> growVisibility(std::vector<bool> visibility);
		void drawLayer(const renderLayer& rl);
		static point2Dd translatePositionToDisplay(point2Dd pos, const double &zoom); //translates a position relative to the camera, to a position relative to the display ready to draw

		void loadTerrainTable();
		
		entt::entity _player;
		entt::entity _camera;
		unsigned int currentBlock=7;

		std::vector<block> _terrainTable;
		universeNode _universeBase;
		entt::registry _enttRegistry;
		
		static std::filesystem::path _savePath;

		void createNewGame(int seed);
		
		void loadGame();
		void saveGame();

		void loadEntities();
		void saveEntities() const;
		void createEntities();
		void fixEntities();


		physicsEngine _physicsEngine;
		std::unique_ptr<std::thread> _chunkLoaderThread;

		static std::mutex endChunkLoader;
		static void _chunkLoaderFunc();
		static universeNode* _chunkLoaderUniverseBase;
		static position* _chunkLoaderPlayerPosition;
		
	};

} // namespace State
