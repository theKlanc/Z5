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
#include "UI/customGadgets/starmap.hpp"
#include "UI/customGadgets/healthDisplay.hpp"
#include "reactPhysics3D/src/reactphysics3d.h"
#include "physicsEngine.hpp"
#include "observer.hpp"

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
		void control(entt::entity);

		struct nodeLayer {
			universeNode* node;
			int layerHeight;
			std::vector<metaBlock> blocks;
			bool firstLayer = false;
		};

		struct renderLayer {
			double depth;
			std::variant<entt::entity, nodeLayer,point3Dd,HI2::Color> target;
		};

		nodeLayer generateNodeLayer(universeNode* node, double depth, fdd localCameraPos);
		void drawLayer(const renderLayer& rl);
		static point2Dd translatePositionToDisplay(point2Dd pos, const double& zoom, const double& spriteZoom = 1); //translates a position relative to the camera, to a position relative to the display ready to draw

		entt::entity _player;
		entt::entity _camera;
		entt::registry _enttRegistry;

		bool _drawDebugInfo = true;

		static sprite* _AOSIDE;
		static sprite* _AOCORNER;

		static std::filesystem::path _savePath;

		void createNewGame(std::string saveName, int seed);

		void loadGame();
		void saveGame();

		void loadEntities();
		void saveEntities() const;
		void createEntities();
		void fixEntities();

		physicsEngine _physicsEngine;

		universeNode _universeBase;
		HI2::Font _standardFont;

		bool _debug = false;

		scene _scene;

		struct {
			std::shared_ptr<healthDisplay> _hDisplay;
			std::shared_ptr<basicTextEntry> _console;
			std::shared_ptr<starmap> _starmap;
		}_sceneElements;



		bool _paused = false;
		bool _step = false;

		void updateCamera();
    };

} // namespace State
