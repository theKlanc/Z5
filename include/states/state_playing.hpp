#pragma once
#include "state_base.hpp"
#include "universeNode.hpp"
#include "entt/entity/registry.hpp"
#include "block.hpp"
#include <variant>
#include <vector>
#include <filesystem>

namespace State {
	class Playing : public virtual State_Base {
	  public:
		Playing();
		Playing(gameCore &gc, std::string );

		void input(float dt) override;
		void update(float dt) override;
		void draw() override;

	  private:
		struct nodeLayer{
			universeNode* node;
			int layerHeight;
		};

		struct renderLayer{
			double depth;
			std::variant<entt::entity,nodeLayer> target;
		};
		void drawLayer(const renderLayer& rl);

		entt::entity _player;
		entt::entity _camera;

		void loadTerrainTable();
		std::vector<block> _terrainTable;
		universeNode _universeBase;
		entt::registry _enttRegistry;

		static point2Dd translatePositionToDisplay(point2Dd pos, const double &zoom); //translates a position relative to the camera, to a position relative to the display ready to draw

		static std::filesystem::path savePath;
		
	};
} // namespace State
