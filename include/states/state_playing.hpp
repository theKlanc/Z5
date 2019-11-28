#pragma once
#include "ui.hpp"
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
		Playing(gameCore& gc, std::string saveName = "default", int seed = -1, bool debug = false);

		void input(double dt) override;
		void update(double dt) override;
		void draw(double dt) override;

		static std::filesystem::path savePath();

		void debugConsoleExec(std::string command);
	private:


		struct nodeLayer {
			universeNode* node;
			int layerHeight;
		};
		struct renderLayer {
			double depth;
			std::variant<entt::entity, nodeLayer> target;
		};
		void drawLayer(const renderLayer& rl);
		static point2Dd translatePositionToDisplay(point2Dd pos, const double& zoom); //translates a position relative to the camera, to a position relative to the display ready to draw

		void loadTerrainTable();

		entt::entity _player;
		entt::entity _camera;
		int selectedBlock = 7;
		blockRotation selectedRotation = UP;

		std::vector<baseBlock> _terrainTable;
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

		HI2::Font _standardFont;

		static std::mutex endChunkLoader;
		static void _chunkLoaderFunc();
		static universeNode* _chunkLoaderUniverseBase;
		static position* _chunkLoaderPlayerPosition;

		bool _debug = false;

		scene _scene;
		std::shared_ptr<basicTextEntry> _console;

	};
} // namespace State
